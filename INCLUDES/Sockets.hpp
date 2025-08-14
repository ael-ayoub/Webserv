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

class Config;


class SockConf
{
	// private:
	
	public:
	int port;
	int fd_socket;
	sockaddr_in addr;
	SockConf(int p);
};
void printVector(std::vector<SockConf> vect);

class Socket
{
private:
	// here vars:
	Methodes m;
	// sockaddr_in addr;
	std::vector<SockConf> sockconf;
	int fd_epoll;

public:
	Socket(std::vector<int>ports);
	int run(Config a);
	void set_nonblocking(int fd);
	void CreateSocket();
	void CreateEpoll();
	int checkEvent(int fd);
	void HandleClient(const int &fd_client, Config a);
	void Monitor(Config a);
};

#endif