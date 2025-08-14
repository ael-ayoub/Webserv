#ifndef SOCKETCONFIG_HPP
#define SOCKETCONFIG_HPP

#include "Webserv.hpp"

class SockConf
{
public:
	int port;
	int fd_socket;
	sockaddr_in addr;
	SockConf(std::string ip, int p);
};

#endif