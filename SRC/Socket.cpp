#include "../INCLUDES/Webserv.hpp"

SockConf::SockConf(std::string ip, int p)
{
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_family = AF_INET;
    addr.sin_port = htons(p);
    fd_socket = -1;
}

Socket::Socket(std::vector<std::pair<std::string, int> > ports)
{
    for (size_t i = 0; i < ports.size(); i++)
    {
        sockconf.push_back(SockConf(ports[i].first, ports[i].second));
    }
}

void Socket::set_nonblocking(int fd)
{
    // int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

void Socket::CreateSocket()
{
    int fd_socket;
    for (size_t i = 0; i < sockconf.size(); i++)
    {
        fd_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_socket == -1)
            throw(std::runtime_error("Cannot create socket!"));

        set_nonblocking(fd_socket);
        sockaddr_in &addr = sockconf[i].addr;

        int opt = 1;
        if (setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        {
            close(fd_socket);
            throw(std::runtime_error("Cannot set socket options!"));
        }

        if (bind(fd_socket, (const sockaddr *)&addr, sizeof(addr)) == -1)
        {
            close(fd_socket);
            throw(std::runtime_error("Cannot bind socket!"));
        }

        if (listen(fd_socket, 10) == -1)
        {
            close(fd_socket);
            throw(std::runtime_error("Cannot put socket in listening state!"));
        }
        sockconf[i].fd_socket = fd_socket;
    }
}

void Socket::CreateEpoll()
{
    epoll_event event;

    fd_epoll = epoll_create(1);
    if (fd_epoll == -1)
        throw(std::runtime_error("Error: cant create epoll instance !!"));

    for (size_t i = 0; i < sockconf.size(); i++)
    {
        event.events = EPOLLIN;
        event.data.fd = sockconf[i].fd_socket;
        if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, sockconf[i].fd_socket, &event) == -1)
        {
            close(sockconf[i].fd_socket);
            close(fd_epoll);
            throw("cannot add socket to epoll instance !");
        }
    }
}

int Socket::checkEvent(int fd)
{
    for (size_t i = 0; i < sockconf.size(); i++)
    {
        if (sockconf[i].fd_socket == fd)
            return (i);
    }
    return (-1);
}

std::string to_string(int num)
{
    std::stringstream ss;
    ss << num;
    return ss.str();
}

std::string format_timestamp(unsigned long long timestamp)
{
    time_t raw_time = timestamp / 1000; // Convert milliseconds to seconds
    struct tm *time_info = localtime(&raw_time);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", time_info);
    return std::string(buffer);
}

std::string generate_log_entry(ClientState &state)
{
    std::string log_entry;
    // log_entry += "Timestamp: " + to_string(state.timestamp) + " | ";
    log_entry += format_timestamp(state.timestamp);
    log_entry += " " + state.method;
    log_entry += " " + state.path;
    log_entry += " " + (state.response.empty() ? "No response generated" : state.response.substr(0, 16) + "...");
    return log_entry;
}

void _print_logs(ClientState &state)
{
    std::string log_entry = generate_log_entry(state);
    std::cout << log_entry << std::endl;
}




void Socket::Monitor(Config &a)
{
    int MAX_EVENTS = 256;
    int fd_client;
    int current_fd;
    int max_fds;
    epoll_event events[MAX_EVENTS];
    epoll_event event_client;
    std::map<int, ClientState> status;

    while (true)
    {
        max_fds = epoll_wait(fd_epoll, events, MAX_EVENTS, 1000);
        if (max_fds == -1)
            throw(std::runtime_error("Error: epoll wait failed !!"));
        if (max_fds == 0)
        {
            // std::cout << "Epoll wait timed out with no events." << std::endl;
            for (std::map<int, ClientState>::iterator it = status.begin();
                 it != status.end();)
            {
                if (!check_timeout(it->second.timestamp, TIMEOUT))
                {
                    int fd = it->first;

                    if (!it->second.send_data)
                    {
                        std::cerr << "\033[1;31m[ERROR]\033[0m Request timeout on fd: " << fd;
                        if (it->second.content_length > 0)
                        {
                            size_t received = it->second.byte_uploaded > 0 ? it->second.byte_uploaded : it->second.body_received;
                            received += it->second.readstring.size();
                            std::cerr << " - Content-Length declared: " << it->second.content_length
                                      << " bytes, but only received: " << received << " bytes"
                                      << " (incomplete body, possible Content-Length mismatch)";
                        }
                        std::cerr << std::endl;
                        it->second.response = ErrorResponse::Error_RequestTimeout(a);
                        it->second.send_data = true;
                        it->second.close = true;
                        it->second.cleanup = true;
                        it->second.waiting = false;
                        event_client.data.fd = fd;
                        event_client.events = EPOLLOUT;
                        epoll_ctl(fd_epoll, EPOLL_CTL_MOD, fd, &event_client);
                        ++it;
                        continue;
                    }

                    epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);

                    std::map<int, ClientState>::iterator to_erase = it;
                    ++it;
                    status.erase(to_erase);
                }
                else
                {
                    ++it;
                }
            }

            continue;
        }

        std::cout << "Epoll returned " << max_fds << " events." << std::endl;
        for (int i = 0; i < max_fds; i++)
        {
            current_fd = events[i].data.fd;
            int index = checkEvent(current_fd);
            if (index != -1)
            {
                std::cout << "Accepting new client on socket fd: " << current_fd << std::endl;
                int &fd_socket = sockconf[index].fd_socket;
                // addr_size = sizeof(addr_client);
                fd_client = accept(fd_socket, NULL, NULL);
                if (fd_client == -1)
                    throw std::runtime_error("cannot accept new client !!");
                set_nonblocking(fd_client);
                event_client.data.fd = fd_client;
                event_client.events = EPOLLIN;
                status[fd_client] = ClientState();
                status[fd_client].timestamp = get_current_timestamp();
                if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_client, &event_client) == -1)
                {
                    close(fd_socket);
                    throw(std::runtime_error("cannot add client to epoll instance !"));
                }
            }
            else
            {
                fd_client = events[i].data.fd;
                std::cout << "Handling client on fd: " << fd_client << std::endl;
                HandleClient(fd_client, a, status);
                std::map<int, ClientState>::iterator it = status.find(fd_client);
                if (it != status.end())
                {
                    if (it->second.waiting)
                    {
                        std::cout << "checking timeout for fd: " << fd_client << std::endl;
                        if (!check_timeout(it->second.timestamp, TIMEOUT))
                        {
                            std::cout << "Connection timed out for fd: " << fd_client << std::endl;
                            if (!it->second.send_data)
                            {
                                std::cerr << "\033[1;31m[ERROR]\033[0m Request timeout on fd: " << fd_client;
                                if (it->second.content_length > 0)
                                {
                                    size_t received = it->second.byte_uploaded > 0 ? it->second.byte_uploaded : it->second.body_received;
                                    received += it->second.readstring.size();
                                    std::cerr << " - Content-Length declared: " << it->second.content_length
                                              << " bytes, but only received: " << received << " bytes"
                                              << " (incomplete body, possible Content-Length mismatch)";
                                }
                                std::cerr << std::endl;
                                it->second.response = ErrorResponse::Error_RequestTimeout(a);
                                it->second.send_data = true;
                                it->second.close = true;
                                it->second.cleanup = true;
                                it->second.waiting = false;
                                event_client.data.fd = fd_client;
                                event_client.events = EPOLLOUT;
                                if (epoll_ctl(fd_epoll, EPOLL_CTL_MOD, fd_client, &event_client) == -1)
                                {
                                    close(fd_client);
                                    perror("epoll_ctl MOD failed");
                                    throw(std::runtime_error("cannot modify client to epoll instance !"));
                                }
                                continue;
                            }
                            cloce_connection(it->second);
                            // continue;
                        }
                        else
                        {
                            it->second.timestamp = get_current_timestamp();
                            std::cout << "Waiting for more data from fd: " << fd_client << std::endl;
                            event_client.data.fd = fd_client;
                            event_client.events = EPOLLIN;
                            if (epoll_ctl(fd_epoll, EPOLL_CTL_MOD, fd_client, &event_client) == -1)
                            {
                                close(fd_client);
                                perror("epoll_ctl MOD failed");
                                throw(std::runtime_error("cannot modify client to epoll instance !"));
                            }
                            continue; // Wait for more data
                        }
                    }
                    if (it->second.send_data)
                    {
                        std::cout << "Sending response to fd: " << fd_client << std::endl;
                        event_client.data.fd = fd_client;
                        event_client.events = EPOLLOUT;
                        if (epoll_ctl(fd_epoll, EPOLL_CTL_MOD, fd_client, &event_client) == -1)
                        {
                            close(fd_client);
                            perror("epoll_ctl MOD failed");
                            throw(std::runtime_error("cannot modify client to epoll instance !"));
                        }
                        try
                        {
                            _sendReaponse(it->second.response, fd_client);
                            // status.erase(it);
                            _print_logs(it->second);
                        }
                        catch (const std::exception &e)
                        {
                            std::cout << "Error sending response to fd: " << fd_client << std::endl;
                            std::cerr << e.what() << '\n';
                            it->second.close = true;
                            it->second.cleanup = true;
                        }
                    }
                    if (it->second.header.find("Connection: close") != std::string::npos || 
                        it->second.response.find("Connection: close") != std::string::npos || 
                        it->second.close)
                    {
                        std::cout << "Closing connection for fd: " << fd_client << std::endl;
                        if (it->second.cleanup)
                            status.erase(it);
                        epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd_client, NULL);
                        close(fd_client);
                    }
                    else
                    {
                        // std::cout << "Keeping connection alive for fd: " << fd_client << std::endl;
                        event_client.data.fd = fd_client;
                        event_client.events = EPOLLIN;
                        if (epoll_ctl(fd_epoll, EPOLL_CTL_MOD, fd_client, &event_client) == -1)
                        {
                            close(fd_client);
                            perror("epoll_ctl MOD failed");
                            throw(std::runtime_error("cannot modify client to epoll instance !"));
                        }
                        it->second.header.clear();
                        it->second.readstring.clear();
                        it->second.metadata.clear();
                        it->second.filename.clear();
                        it->second.boundary.clear();
                        it->second.method.clear();
                        it->second.path.clear();
                        it->second = ClientState();
                    }
                }
            }
        }
    }
}

void Socket::run(Config &a)
{
    try
    {
        CreateSocket();
        CreateEpoll();
        Monitor(a);
    }
    catch (std::exception &e)
    {
        std::cout << "cannod run the rerver !!!" << std::endl;
        std::cout << e.what() << std::endl;
    }
}
