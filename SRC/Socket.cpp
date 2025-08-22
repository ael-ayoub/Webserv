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

void Socket::HandleClient(const int &fd_client, Config a)
{
    char buffer[1024];
    int b_read = read(fd_client, buffer, 1024);
    if (b_read > 0)
    {
        if (b_read >= 1024) b_read = 1023;
        buffer[b_read] = '\0';
        std::cout << buffer << std::endl;
        std::vector<ServerConfig> servers;
        size_t i = 0;
        servers = a.get_allserver_config();
        std::pair<std::string, int> ip_port;
        Request test_request;
        std::string response;
        
        try {
            response = test_request.parse_request(buffer, a);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing request: " << e.what() << std::endl;
            response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nContent-Length: 11\r\n\r\nBad Request";
        }
        
        while (i < servers.size())
        {
            ip_port = servers[i].get_ip();
            if (ip_port.first == test_request.get_Hostname()
                && ip_port.second == test_request.get_port())
                break;
            i++;
        }

        if (i >= servers.size()) {
            response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 9\r\n\r\nNot Found";
        }

        if (response == "NONE" && i < servers.size())
        {
            try {
                if (test_request.get_method() == "GET" || test_request.get_method() == "DELETE") {
                    response = m.GetMethod(a, test_request, servers[i]);
                } else if (test_request.get_method() == "POST") {
                    LocationConfig info_location = servers[i].get_conf(test_request.get_path());
                    int post_status = m.PostMethod(buffer, info_location);
                    if (post_status == 0)
                        response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 7\r\n\r\nSuccess";
                    else
                        response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n\r\nError";
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing request: " << e.what() << std::endl;
                response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nContent-Length: 21\r\n\r\nInternal Server Error";
            }
        }
        size_t total_sent = 0;
        while (total_sent < response.size())
        {
            ssize_t sent = send(fd_client, response.c_str() + total_sent,
                                response.size() - total_sent, 0);
            if (sent <= 0)
                break;
            total_sent += sent;
        }
    }
    else if (b_read == 0)
    {
        std::cout << "Client closed connection" << std::endl;
        epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd_client, NULL);
        close(fd_client);
    }
    else
    {
        std::cerr << "Read error from client" << std::endl;
        epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd_client, NULL);
        close(fd_client);
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

void Socket::Monitor(Config a)
{
    int MAX_EVENTS = 256;
    int fd_client;
    int current_fd;
    socklen_t addr_size;
    int max_fds;
    epoll_event events[MAX_EVENTS];
    epoll_event event_client;
    sockaddr_in addr_client;
    (void) addr_size;
    while (true)
    {
        max_fds = epoll_wait(fd_epoll, events, MAX_EVENTS, -1);
        for (int i = 0; i < max_fds; i++)
        {
            current_fd = events[i].data.fd;
            int index = checkEvent(current_fd);
            if (index != -1)
            {
                int &fd_socket = sockconf[index].fd_socket;
                addr_size = sizeof(addr_client);
                fd_client = accept(fd_socket, NULL, NULL);
                if (fd_client == -1)
                    throw std::runtime_error("cannot accept new client !!");

                event_client.data.fd = fd_client;
                event_client.events = EPOLLIN;
                set_nonblocking(fd_client);
                if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_client, &event_client) == -1)
                {
                    close(fd_socket);
                    throw(std::runtime_error("cannot add client to epoll instance !"));
                }
            }
            else
            {
                fd_client = events[i].data.fd;
                HandleClient(fd_client, a);
            }
        }
    }
}

void Socket::run(Config a)
{
    try
    {
        CreateSocket();
        CreateEpoll();
        Monitor(a);
    }
    catch (std::exception &e)
    {
        std::cout << "cannot run the rerver !!!" << std::endl;
        std::cout << e.what() << std::endl;
    }
}
