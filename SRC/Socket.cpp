#include "../includes/Webserv.hpp"
#include "../includes/CGI.hpp"

SockConf::SockConf(std::string ip, int p)
{
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_family = AF_INET;
    addr.sin_port = htons(p);
    fd_socket = -1;
}

Socket::Socket(std::vector<std::pair<std::string, int> > ports)
{
    // for (size_t i = 0; i < ports.size(); i++)
    // {
    for (size_t j = 0; j < ports.size(); j++)
    {
        sockconf.push_back(SockConf(ports[j].first, ports[j].second));
        // sockconf.push_back(SockConf(ports[i][j].first, ports[i][j].second));
    }
    // }
}

void Socket::set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
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
    time_t raw_time = timestamp / 1000;
    struct tm *time_info = localtime(&raw_time);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", time_info);
    return std::string(buffer);
}

std::string makeLog(ClientState &state)
{
    std::string log_entry;

    std::string stats = "";
    if (state.response.empty())
        stats = "No response generated";
    else
        stats = state.response.substr(0, 16);

    log_entry += format_timestamp(state.timestamp);
    log_entry += " " + state.method;
    log_entry += " " + state.path;
    log_entry += " " + stats;
    return log_entry;
}

void printLogs(ClientState &state)
{
    std::string log_entry = makeLog(state);
    std::cout << log_entry << std::endl;
}

void sendAndClose(ClientState &state, const std::string &response)
{
    state.response = response;
    state.send_data = true;
    state.close = true;
    state.cleanup = true;
    state.waiting = false;
}

void requestnotComplet(int fd, ClientState &state)
{
    std::cerr << "\033[1;31m[ERROR]\033[0m Request timeout on fd: " << fd;
    if (state.content_length > 0)
    {
        size_t received = state.byte_uploaded > 0 ? state.byte_uploaded : state.body_received;
        received += state.readstring.size();
        std::cerr << " - Content-Length declared: " << state.content_length
                  << " bytes, but only received: " << received << " bytes"
                  << " (incomplete body, possible Content-Length mismatch)";
    }
    std::cerr << std::endl;
}

bool modClientEvent(int fd_epoll, int fd, uint32_t events)
{
    epoll_event event_client;
    event_client.data.fd = fd;
    event_client.events = events;
    return epoll_ctl(fd_epoll, EPOLL_CTL_MOD, fd, &event_client) != -1;
}

void modClientEventOrThrow(int fd_epoll, int fd, uint32_t events)
{
    if (!modClientEvent(fd_epoll, fd, events))
    {
        close(fd);
        std::cerr << "epoll_ctl MOD failed" << std::endl;
        throw(std::runtime_error("cannot modify client to epoll instance !"));
    }
}

void rmCgiFdFromMap(int fd_epoll, int cgi_fd, std::map<int, int> &cgi_to_client)
{
    epoll_ctl(fd_epoll, EPOLL_CTL_DEL, cgi_fd, NULL);
    close(cgi_fd);
    cgi_to_client.erase(cgi_fd);
}

void resetKeepAliveState(ClientState &state)
{
    std::string saved_ip = state.ip;
    int saved_port = state.port;

    state.header.clear();
    state.readstring.clear();
    state.metadata.clear();
    state.filename.clear();
    state.boundary.clear();
    state.method.clear();
    state.path.clear();
    state.response.clear();
    state = ClientState();
    state.ip = saved_ip;
    state.port = saved_port;
    state.timestamp = get_current_timestamp();
    state.byte_send = 0;
    state.send_complete = false;
}

void Socket::Monitor(Config &a)
{
    int MAX_EVENTS = 256;
    int fd_client;
    int current_fd;
    int max_fds;
    epoll_event events[MAX_EVENTS];
    std::map<int, ClientState> status;
    std::map<int, int> cgi_to_client;
    const unsigned long long CGI_TIMEOUT_MS = 5000;

    while (true)
    {
        max_fds = epoll_wait(fd_epoll, events, MAX_EVENTS, 1000);
        if (max_fds == -1)
            throw(std::runtime_error("Error: epoll wait failed !!"));
        if (max_fds == 0)
        {
            for (std::map<int, ClientState>::iterator it = status.begin();
                 it != status.end();)
            {
                if (it->second.cgi_active && !check_timeout(it->second.cgi_deadline, CGI_TIMEOUT_MS))
                {
                    cleanup_cgi_state(it->second, fd_epoll, cgi_to_client);
                    sendAndClose(it->second, ErrorResponse::Error_GatewayTimeout(a));
                    modClientEvent(fd_epoll, it->first, EPOLLOUT);
                    ++it;
                    continue;
                }

                if (it->second.cgi_active)
                {
                    ++it;
                    continue;
                }

                if (!check_timeout(it->second.timestamp, TIMEOUT))
                {
                    int fd = it->first;

                    if (!it->second.send_data)
                    {
                        if (it->second.complete_header || !it->second.readstring.empty())
                        {
                            requestnotComplet(fd, it->second);
                            sendAndClose(it->second, ErrorResponse::Error_RequestTimeout(a));
                            modClientEvent(fd_epoll, fd, EPOLLOUT);
                            ++it;
                            continue;
                        }
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

        for (int i = 0; i < max_fds; i++)
        {
            current_fd = events[i].data.fd;
            int index = checkEvent(current_fd);
            if (index != -1)
            {
                int &fd_socket = sockconf[index].fd_socket;
                fd_client = accept(fd_socket, NULL, NULL);
                if (fd_client == -1)
                    throw std::runtime_error("cannot accept new client !!");
                set_nonblocking(fd_client);
                status[fd_client] = ClientState();
                status[fd_client].ip = inet_ntoa(sockconf[index].addr.sin_addr);
                status[fd_client].port = ntohs(sockconf[index].addr.sin_port);
                status[fd_client].timestamp = get_current_timestamp();
                epoll_event event_client;
                event_client.data.fd = fd_client;
                event_client.events = EPOLLIN;
                if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_client, &event_client) == -1)
                {
                    close(fd_socket);
                    throw(std::runtime_error("cannot add client to epoll instance !"));
                }
            }
            else if (cgi_to_client.find(current_fd) != cgi_to_client.end())
            {
                int owner_fd = cgi_to_client[current_fd];
                std::map<int, ClientState>::iterator owner = status.find(owner_fd);
                if (owner == status.end())
                {
                    rmCgiFdFromMap(fd_epoll, current_fd, cgi_to_client);
                    continue;
                }

                ClientState &cstate = owner->second;
                if (!cstate.cgi_active)
                {
                    rmCgiFdFromMap(fd_epoll, current_fd, cgi_to_client);
                    continue;
                }

                if (!check_timeout(cstate.cgi_deadline, CGI_TIMEOUT_MS))
                {
                    cleanup_cgi_state(cstate, fd_epoll, cgi_to_client);
                    sendAndClose(cstate, ErrorResponse::Error_GatewayTimeout(a));
                    modClientEvent(fd_epoll, owner_fd, EPOLLOUT);
                    continue;
                }

                if (events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
                {
                    if (cstate.cgi_output.empty())
                    {
                        cleanup_cgi_state(cstate, fd_epoll, cgi_to_client);
                        sendAndClose(cstate, ErrorResponse::Error_Internal_Server(a));
                    }
                    else
                        finalize_cgi_success(cstate, fd_epoll, cgi_to_client);
                    modClientEvent(fd_epoll, owner_fd, EPOLLOUT);
                    continue;
                }

                char buffer[4096];
                ssize_t n = read(current_fd, buffer, sizeof(buffer));
                if (n > 0)
                {
                    cstate.cgi_output.append(buffer, (size_t)n);
                    cstate.timestamp = get_current_timestamp();
                    continue;
                }
                if (n == 0)
                {
                    finalize_cgi_success(cstate, fd_epoll, cgi_to_client);
                    modClientEvent(fd_epoll, owner_fd, EPOLLOUT);
                    continue;
                }

                continue;
            }
            else
            {
                fd_client = events[i].data.fd;
                HandleClient(fd_client, a, status);
                std::map<int, ClientState>::iterator it = status.find(fd_client);
                if (it != status.end())
                {
                    if (it->second.cgi_active && it->second.cgi_fd != -1 && cgi_to_client.find(it->second.cgi_fd) == cgi_to_client.end())
                    {
                        epoll_event cgi_event;
                        cgi_event.data.fd = it->second.cgi_fd;
                        cgi_event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
                        if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, it->second.cgi_fd, &cgi_event) == -1)
                        {
                            cleanup_cgi_state(it->second, fd_epoll, cgi_to_client);
                            sendAndClose(it->second, ErrorResponse::Error_Internal_Server(a));
                        }
                        else
                            cgi_to_client[it->second.cgi_fd] = fd_client;
                    }

                    if (it->second.waiting)
                    {
                        if (it->second.cgi_active)
                            continue;
                        if (!check_timeout(it->second.timestamp, TIMEOUT))
                        {
                            if (!it->second.send_data)
                            {
                                requestnotComplet(fd_client, it->second);
                                sendAndClose(it->second, ErrorResponse::Error_RequestTimeout(a));
                                modClientEventOrThrow(fd_epoll, fd_client, EPOLLOUT);
                                continue;
                            }
                            cloce_connection(it->second);
                        }
                        else
                        {
                            it->second.timestamp = get_current_timestamp();
                            modClientEventOrThrow(fd_epoll, fd_client, EPOLLIN);
                            continue;
                        }
                    }
                    if (it->second.send_data && !it->second.send_complete)
                    {
                        modClientEventOrThrow(fd_epoll, fd_client, EPOLLOUT);

                        ssize_t sent = send(fd_client,
                                            it->second.response.c_str() + it->second.byte_send,
                                            it->second.response.size() - it->second.byte_send,
                                            MSG_NOSIGNAL);

                        if (sent > 0)
                        {
                            it->second.byte_send += sent;
                            if (it->second.byte_send >= it->second.response.size())
                            {
                                it->second.send_complete = true;
                                printLogs(it->second);
                            }
                        }
                        else if (sent < 0)
                        {
                            it->second.close = true;
                            it->second.cleanup = true;
                        }
                        else
                        {
                            it->second.close = true;
                            it->second.cleanup = true;
                        }
                    }
                    else if (it->second.send_data && it->second.send_complete)
                    {
                        if (it->second.header.find("Connection: close") != std::string::npos ||
                            it->second.response.find("Connection: close") != std::string::npos ||
                            it->second.close)
                        {
                            if (it->second.cgi_active)
                                cleanup_cgi_state(it->second, fd_epoll, cgi_to_client);
                            if (it->second.cleanup)
                                status.erase(it);
                            epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd_client, NULL);
                            close(fd_client);
                        }
                        else
                        {
                            modClientEventOrThrow(fd_epoll, fd_client, EPOLLIN);
                            resetKeepAliveState(it->second);
                        }
                    }
                    else if (it->second.header.find("Connection: close") != std::string::npos ||
                             it->second.response.find("Connection: close") != std::string::npos ||
                             it->second.close)
                    {
                        if (it->second.cgi_active)
                            cleanup_cgi_state(it->second, fd_epoll, cgi_to_client);
                        if (it->second.cleanup)
                            status.erase(it);
                        epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd_client, NULL);
                        close(fd_client);
                    }
                    else
                    {
                        modClientEventOrThrow(fd_epoll, fd_client, EPOLLIN);
                        resetKeepAliveState(it->second);
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
