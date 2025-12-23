#ifndef RESPONSE_H
#define RESPONSE_H

#include "Webserv.hpp"

class Config;
class Request;
class ServerConfig;
class Methodes
{
	public:
		std::string GetMethod(Config a, Request test_request, ServerConfig Servers_Config);
		// void PostMethod(); 
		// std::string GetMethod(); 
		std::string PostMethod(Config& a, Request test_request, ServerConfig Servers_Config,const int &fd, const std::string &header); 
		// std::string ft_getline(int fd);

		// void DeleteMethod(); 
};

#endif 