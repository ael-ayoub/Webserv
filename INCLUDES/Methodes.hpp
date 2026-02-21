#ifndef RESPONSE_H
#define RESPONSE_H

#include "Webserv.hpp"


class Config;
class Request;
class ServerConfig;
class ClientState;
class Methodes
{
	public:
		std::string GetMethod(Config& a, Request& request, ServerConfig& Servers_Config); 
		std::string PostMethod(Config& a, const int &fd_client,ClientState &state);
};

#endif 