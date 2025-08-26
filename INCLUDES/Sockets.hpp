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
#include "socketConfig.hpp"

class Config;


class Socket
{
private:
	Methodes m;
	std::vector<SockConf> sockconf;
	int fd_epoll;

public:
	Socket(std::vector<std::pair<std::string, int> > ports);
	void run(Config &a);
	void set_nonblocking(int fd);
	void CreateSocket();
	void CreateEpoll();
	int checkEvent(int fd);
	void HandleClient(const int &fd_client, Config& a);
	void Monitor(Config &a);
};

#endif