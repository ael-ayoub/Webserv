#include "../INCLUDES/Sockets.hpp"

void set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int CreateSocket(const sockaddr_in &addr)
{
    int fd_socket;

    fd_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_socket == -1)
    {
        std::cerr << "Cannot create socket!" << std::endl;
        return -1;
    }

    set_nonblocking(fd_socket);

    int opt = 1;
    if (setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "Cannot set socket options!" << std::endl;
        close(fd_socket);
        return -1;
    }

    if (bind(fd_socket, (const sockaddr *)&addr, sizeof(addr)) == -1)
    {
        std::cerr << "Cannot bind socket!" << std::endl;
        close(fd_socket);
        return -1;
    }

    if (listen(fd_socket, 10) == -1)
    {
        std::cerr << "Cannot put socket in listening state!" << std::endl;
        close(fd_socket);
        return -1;
    }

    return fd_socket;
}

int CreateEpoll(int fd_socket)
{
    int fd_epoll;
    epoll_event event;

    fd_epoll = epoll_create(1);
    if (fd_epoll == -1)
    {
        std::cerr << "Error: cant create epoll instance !!" << std::endl;
        close(fd_socket);
        return (-1);
    }

    event.events = EPOLLIN;
    event.data.fd = fd_socket;
    if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_socket, &event) == -1)
    {
        std::cerr << "cannot add socket to epoll instance !" << std::endl;
        close(fd_socket);
        close(fd_epoll);
        return (-1);
    }
    return (fd_epoll);
}
 
void HandleClient(const int &fd_epoll, const int &fd_client)
{
    // this just simple handling of client !!!!!!!

    // so i get file that conation message of request : 

    // it is fd_Client;
    // so should parse it , for prepare responce !!!

    char buffer[1024];
    int b_read = read(fd_client, buffer, 1024);
    if (b_read > 0)
    {
        buffer[b_read] = '\0';
        std::cout << buffer;
    }
    else
    {
        epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd_client, NULL);
        std::cout << "cant read from the file ..." << std::endl;
        close(fd_client);
    }
}

void  Monitor(const int &fd_socket, const int &fd_epoll, sockaddr_in &addr)
{
    int MAX_EVENTS = 256;
    int fd_client;
    int current_fd;
    socklen_t addr_size;
    int max_fds;
    epoll_event events[MAX_EVENTS];
    epoll_event event_client;

    while (true)
    {
        max_fds = epoll_wait(fd_epoll, events, MAX_EVENTS, -1);
        for (int i = 0; i < max_fds; i++)
        {
            current_fd = events[i].data.fd;
            if (current_fd == fd_socket)
            {
                std::cout << "new connection ..." << std::endl;
                addr_size = sizeof(addr);
                fd_client = accept(fd_socket, (sockaddr *)&addr, &addr_size);
                event_client.data.fd = fd_client;
                event_client.events = EPOLLIN;
                set_nonblocking(fd_client);
                if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_client, &event_client) == -1)
                {
                
                    std::cerr << "cannot add client to epoll instance !" << std::endl;
                    close(fd_socket);
                    close(fd_epoll);
                    return ;
                }
            }
            else
            {
                fd_client = events[i].data.fd;
                HandleClient(fd_epoll, fd_client);
            }
        }
    }
}

