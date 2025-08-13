#ifndef RESPONSE_H
#define RESPONSE_H

#include "Webserv.hpp"

class Methodes
{
	public:
		std::string GetMethod(); 
		int PostMethod(const std::string& Prequest); 
		void DeleteMethod(); 
};

#endif 