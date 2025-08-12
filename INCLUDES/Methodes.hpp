#ifndef RESPONSE_H
#define RESPONSE_H

#include "Webserv.hpp"

class Config;
class Methodes
{
	public:
		std::string GetMethod(Config a, char *buffer);
		void PostMethod(); 
		void DeleteMethod(); 
};

#endif 