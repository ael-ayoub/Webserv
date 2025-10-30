#include "../../INCLUDES/Webserv.hpp"

#include <sstream>

std::string generat_random_id()
{
	static const char charset[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string random;
	for (size_t i = 0; i < 11; i++)
	{
		random += charset[rand() % sizeof(charset) - 1];
	}
	return random;
}

std::string  Methodes::PostMethod(Config &a, Request test_request, ServerConfig Servers_Config , int fd_client,const std::string &header)
{
	(void)a;
	//(void)test_request;
	(void)Servers_Config;
	std::string response;
	std::cout << "this responce: " << std::endl;


	if (test_request.get_path() == "/register")
	{
		// User user = test_request.get_user();
		if (test_request.check_if_user_exist())
		{
			std::string body = "<html><body> <h1>User already exists!</h1></body></html>";
			std::stringstream ss;
			ss << body.size();
			response =
				"HTTP/1.1 409 Conflict\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " +
				ss.str() + "\r\n"
						   "Connection: close\r\n"
						   "\r\n" + body;
		}
		else
		{
			std::string body = "<html>\n\t<body>\n\t<h1>Registration successful. Welcome!</h1>\n\t</body>\n</html>";
			test_request.save_user_in_data();
			std::stringstream ss;
			ss << body.size();
			response =
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " +
				ss.str() + "\r\n"
				"Connection: close\r\n"
			   	"\r\n" +
				body;
		}
	}
	else if (test_request.get_path() == "/login")
	{
		if (a.check_session(test_request.get_session()))
        {
            std::string body = "<html><body> <h1>u are loger in !!</h1></body></html>";
			std::stringstream ss;
			ss << body.size();
			std::string response =
				"HTTP/1.1 409 Conflict\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " +
				ss.str() + "\r\n"
						   "Connection: close\r\n"
						   "\r\n" +
				body;
                // return response;
        }
		if (test_request.check_if_user_exist())
		{
			srand(time(0));
			std::string id = generat_random_id();
			User u = test_request.get_user();
			a.set_sessions(std::make_pair(u.getUsername(),id));
			std::string body = "<html>\n\t<body>\n\t<h1>Login successful. Welcome!</h1>\n\t</body>\n</html>";
			std::stringstream ss;
			ss << body.size();
			response =
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " + ss.str()  + "\r\n"
				"Set-Cookie: " + u.getUsername() + "=" + id + "\r\n"
				"Connection: close\r\n" 
				"\r\n" + body;
		}
		else
		{
			User u = test_request.get_user();
			std::string body = "<html><body> <h1>User do not exists!</h1></body></html>";
			std::stringstream ss;
			ss << body.size();
			srand(time(0));
			response =
				"HTTP/1.1 409 Conflict\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " + ss.str() + "\r\n" 
				"Connection: close\r\n"
				"\r\n" + body;
		}
	}
	else if (test_request.get_path() == "/uploads")
	{
		// std::cout << "i am here prob " << std::endl;
		// size_t pos = header.find()
		std::string metadata;

		
		std::string body = "<html><body> <h1>User dzxczxczxo not exists!</h1></body></html>";
		std::stringstream ss;
		ss << body.size();
		srand(time(0));
		// usleep( 1000);
		response =
		"HTTP/1.1 409 Conflict\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + ss.str() + "\r\n" 
		"Connection: close\r\n\r\n";
		response += body;
	}
	(void )fd_client;
	// close(fd_client);
	return response;
}
