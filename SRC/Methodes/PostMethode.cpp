#include "../../INCLUDES/Webserv.hpp"


int Methodes::PostMethod(const std::string& Prequest)
{
	if (Prequest.empty())
		throw(std::invalid_argument("Post request is empty !"));
	//should get type and name 
	std::string path = "/home/ael-aiss/Desktop/Webserv/www/uploads/filename.txt";
	std::ofstream fileUpload(path.c_str(), std::ios::app);

	if (!fileUpload.is_open())
		throw(std::invalid_argument("cannt create file !!"));

	size_t boundary_satart = Prequest.find("-----");
	size_t boundary_end = Prequest.find("\r\n\r\n");
	std::string boundary = Prequest.substr(boundary_satart, boundary_end);
	std::cout << boundary << std::endl;
	size_t end = Prequest.find(boundary, boundary_end);
	std::string body  = Prequest.substr(boundary_end + 5, end);
	std::cout << "*******************" << std::endl;
	std::cout << body << std::endl;
	fileUpload << body;
	std::cout << "*******************" << std::endl;
	return (1);
}
