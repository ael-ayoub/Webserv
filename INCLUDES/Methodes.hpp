#ifndef RESPONSE_H
#define RESPONSE_H

#include "Webserv.hpp"

class Methodes
{
	public:
		std::string GetMethod(); 
		void PostMethod(); 
		void DeleteMethod(); 
};

#endif 