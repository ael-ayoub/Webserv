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

static std::string ft_getline(int fd)
{
	if (fd < 0)
		return "";

	char ch;
	int byte_read;
	std::string line;
	while ((byte_read = read(fd, &ch, 1)) > 0)
	{
		if (ch == '\n')
			break;
		line += ch;
	}
	return line;
}

std::string Methodes::PostMethod(Config &a, Request test_request, ServerConfig Servers_Config, const int &fd_client, const std::string &header)
{
	(void)a;
	//(void)test_request;
	(void)Servers_Config;
	std::string response;

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
						   "\r\n" +
				body;
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
			a.set_sessions(std::make_pair(u.getUsername(), id));
			std::string body = "<html>\n\t<body>\n\t<h1>Login successful. Welcome!</h1>\n\t</body>\n</html>";
			std::stringstream ss;
			ss << body.size();
			response =
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " +
				ss.str() + "\r\n"
						   "Set-Cookie: " +
				u.getUsername() + "=" + id + "\r\n"
											 "Connection: close\r\n"
											 "\r\n" +
				body;
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
				"Content-Length: " +
				ss.str() + "\r\n"
						   "Connection: close\r\n"
						   "\r\n" +
				body;
		}
	}
	else if (test_request.get_path() == "/uploads")
	{
		std::string boundary;
		size_t pos = header.find("boundary=");
		if (pos != std::string::npos)
		{
			boundary = header.substr(pos + sizeof("boundary=") - 1, header.find("\n", pos) - sizeof("boundary=") - pos);
		}
		std::string metadata, line;
		while ((line = ft_getline(fd_client)) != "")
		{
			if (line == "\r" || line.empty())
				break;
			metadata += line + "\n";
		}
		size_t pos_3 = metadata.find("filename=");
		std::string filename;
		if (pos_3 != std::string::npos)
		{
			filename = metadata.substr(pos_3 + sizeof("filename="), metadata.find("\n", pos_3) - sizeof("filename=") - pos_3 - 2);
			// printf("======\n___%s___\n=======\n", filename.c_str());
		}
		std::string u_path = "STATIC/upload/" + filename;

		int fd = open(u_path.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666);

		if (fd < 0)
		{
			std::cout << "problem whele try to open file ..." << std::endl;
		}
		else
		{
			std::string end_boundary = "--" + boundary + "--";

			char buffer[4096];
			std::vector<char> tail; // ← Changed to vector
			bool found = false;

			while (!found)
			{
				ssize_t byte_read = read(fd_client, buffer, sizeof(buffer));
				if (byte_read <= 0)
				{
					if (byte_read < 0)
						std::cout << "read function failed" << std::endl;
					break;
				}

				// Combine tail + new data
				std::vector<char> check;
				check.reserve(tail.size() + byte_read);
				check.insert(check.end(), tail.begin(), tail.end());
				check.insert(check.end(), buffer, buffer + byte_read);

				// Search for boundary (convert to string only for searching)
				std::string check_str(check.begin(), check.end());
				size_t pos = check_str.find(end_boundary);

				if (pos != std::string::npos)
				{
					found = true;
					if (write(fd, check.data(), pos) == -1)
					{
						std::cout << "write failed" << std::endl;
						break;
					}
				}
				else
				{
					size_t write_size = check.size();

					if (write_size >= end_boundary.size())
					{
						write_size = check.size() - (end_boundary.size() - 1);
						tail.assign(check.begin() + write_size, check.end());
					}
					else
					{
						tail = check;
						continue;
					}

					if (write(fd, check.data(), write_size) == -1)
					{
						std::cout << "write failed" << std::endl;
						break;
					}
				}
			}
			close(fd);
		}

		std::string body = "<html><body> <h1>file uploaded with sccuss !</h1></body></html>";
		std::stringstream ss;
		ss << body.size();
		response =
			"HTTP/1.1 409 Conflict\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: " +
			ss.str() + "\r\n"
					   "Connection: close\r\n\r\n";
		response += body;
		close(fd);
	}
	return response;
}
