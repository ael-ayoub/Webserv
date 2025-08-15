#ifndef RESPONSE_H
#define RESPONSE_H

#include "Webserv.hpp"

class Config;
class Request;
class ServerConfig;
class Methodes
{
	public:
		static std::string GetMethod(Config a, Request test_request, ServerConfig Servers_Config);
		void PostMethod(); 
		std::string GetMethod(); 
		int PostMethod(const std::string& Prequest); 
		void DeleteMethod(); 
};

#endif 