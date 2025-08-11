#include "INCLUDES/Webserv.hpp"

int main()
{
    try
    {
        Config a;
        a.store_file("/home/abahaded/Desktop/Webserv/SRC/default.conf");
        // int fd_client;
        int fd_socket;
        int fd_epoll;
        /***** remarque: we will be take these parameters from config file */
        // i will fixe it 
    
        sockaddr_in addr;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8080);
    
        fd_socket = CreateSocket(addr);
        if (fd_socket == -1)
            return (0);
        fd_epoll = CreateEpoll(fd_socket);
        if (fd_epoll == -1)
            return (0);

        // now just go to monitor function  u will see function named handleclient ( this function conatian request )to see request: 
        Monitor(fd_socket, fd_epoll, addr, a);
    }
    catch(const Config::ErrorSyntax& e)
    {
        std::cerr << e.what() << '\n';
    }

}
