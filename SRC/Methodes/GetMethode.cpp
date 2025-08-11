#include "../../INCLUDES/Webserv.hpp"


std::string Methodes::GetMethod()
{
	std::ifstream file("/home/ael-aiss/Desktop/Webserv/www/index.html", std::ios::in);
	if (!file.is_open())
	{
		return "";
		// here should send error message or page depend on our implimentation;
	}

	std::string body, line;
	while (getline(file, line))
		body += line + "\n";

	std::stringstream ss ;
	ss << body.size();
	std::string s = ss.str();
	std::string statusLine = "HTTP/1.1 200 OK\r\n";

	std::string header =
		"Content-Type: text/html\r\n"
		"Content-Length: " + s + "\r\n"
		"Date: Sun, 10 Aug 2025 16:45:00 GMT\r\n"
		"Server: webserv42/1.0\r\n"
		"Connection: close\r\n";

	std::string response;
	response += statusLine;
	response += header;
	response += "\r\n";
	response += body;

	return response;
}
