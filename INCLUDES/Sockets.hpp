#ifndef SOCKETS_HPP
#define SOCKETS_HPP

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <iostream>
#include "Webserv.hpp"

class Socket
{
private:
	// here vars:
	Methodes m;
	sockaddr_in addr;
public:
	Socket();
	int run();
	void set_nonblocking(int fd);
	int CreateSocket();
	int CreateEpoll(int fd_socket);
	void HandleClient(const int &fd_epoll, const int &fd_client);
	void Monitor(const int &fd_socket, const int &fd_epoll);
};

#endif