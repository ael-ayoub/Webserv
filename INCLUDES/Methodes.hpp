#ifndef RESPONSE_H
#define RESPONSE_H

#include "Webserv.hpp"
// #include "Sockets.hpp"


class Config;
class Request;
class ServerConfig;
class ClientState;
class Methodes
{
	public:
		std::string GetMethod(Config& a, Request& request, ServerConfig& Servers_Config);
		// void PostMethod(); 
		// std::string GetMethod(); 
		// std::string PostMethod(Config& a, Request test_request,const int &fd,ClientState &state); 
		std::string PostMethod(Config& a, const int &fd_client,ClientState &state);
		// std::string ft_getline(int fd);

		// void DeleteMethod(); 
};

#endif 