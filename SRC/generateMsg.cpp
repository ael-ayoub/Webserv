#include "../INCLUDES/Webserv.hpp"


std::string generateSuccessMsg()
{
    std::string body = "<html><body><h1>File uploaded successfully!</h1></body></html>";
	std::stringstream ss;
	ss << body.size();
	std::string success =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " +
		ss.str() + "\r\n"
				   "Connection: close\r\n\r\n";
	success += body;

    return success;
}

std::string generateFailerMsg()
{
    std::string body = "<html><body><h1>File upload failed!</h1></body></html>";
	std::stringstream ss2;
	ss2 << body.size();
	std::string fail =
		"HTTP/1.1 409 Conflict\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " +
		ss2.str() + "\r\n"
					"Connection: close\r\n\r\n";
	fail += body;
    return fail;
}