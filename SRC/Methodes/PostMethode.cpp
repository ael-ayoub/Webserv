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
//@hello 
static std::string ft_getline(int fd)
{
	if (fd < 0)
		return "";

	char ch;
	int byte_read;
	std::string line;
	while ((byte_read = read(fd, &ch, 1)) > 0)
	{
		if (byte_read < 0)
		{
			std::cerr << "Error in read: " << strerror(errno) << std::endl;
			return "";
		}
		if (ch == '\n')
			break;
		line += ch;
	}
	return line;
}

void Upload_files(std::string &response, const std::string &header, const int &fd_client)
{
	if (fd_client < 0)
	{
		std::cerr << "Error: Invalid client file descriptor" << std::endl;
		return;
	}

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

	body = "<html><body><h1>File upload failed!</h1></body></html>";
	std::stringstream ss2;
	ss2 << body.size();
	std::string fail =
		"HTTP/1.1 409 Conflict\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " +
		ss2.str() + "\r\n"
					"Connection: close\r\n\r\n";
	fail += body;

	std::string boundary;
	size_t pos = header.find("boundary=");
	if (pos != std::string::npos)
	{
		pos += 9;
		size_t end = header.find("\n", pos);
		if (end != std::string::npos)
			boundary = header.substr(pos, end - pos);
	}

	std::string metadata;
	std::string line;
	while (true)
	{
		line = ft_getline(fd_client);
		if (line == "\r")
			break;
		metadata += line + "\n";
	}

	std::string filename;
	size_t filename_pos = metadata.find("filename=\"");
	if (filename_pos != std::string::npos)
	{
		filename_pos += 10;
		size_t end_quote = metadata.find("\"", filename_pos);
		if (end_quote != std::string::npos)
			filename = metadata.substr(filename_pos, end_quote - filename_pos);
	}

	if (filename.empty())
	{
		std::cerr << "Error: No filename found in upload" << std::endl;
		response = fail;
		return;
	}

	std::string u_path = "STATIC/upload/" + filename;
	int fd = open(u_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);

	if (fd < 0)
	{
		std::cerr << "Error: Failed to open/create file '" << u_path << "': "
				  << strerror(errno) << std::endl;
		response = fail;
		return;
	}
	else
	{
		std::string end_boundary = "--" + boundary + "--";

		char buffer[4096];
		std::vector<char> tail;
		bool found = false;

		while (!found)
		{
			ssize_t byte_read = read(fd_client, buffer, sizeof(buffer));
			if (byte_read <= 0)
			{
				if (byte_read < 0)
					std::cout << "read function failed" << std::endl;
				close(fd);
				response = fail;
				return;
			}

			std::vector<char> check;
			check.reserve(tail.size() + byte_read);
			check.insert(check.end(), tail.begin(), tail.end());
			check.insert(check.end(), buffer, buffer + byte_read);

			std::string check_str(check.begin(), check.end());
			size_t pos = check_str.find(end_boundary);

			if (pos != std::string::npos)
			{
				found = true;
				if (write(fd, check.data(), pos) == -1)
				{
					std::cerr << "Error: Failed to write to file '" << u_path << "': "
							  << strerror(errno) << std::endl;
					close(fd);
					response = fail;
					return;
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
					std::cerr << "Error: Failed to write to file '" << u_path << "': "
							  << strerror(errno) << std::endl;
					close(fd);
					response = fail;
					return;
				}
			}
		}
		close(fd);
	}
	response = success;
}

std::string Methodes::PostMethod(Config& a, const int &fd_client,ClientState &state)
{
	if (state.path != "/uploads")
	{
		return ErrorResponse::Error_NotFound(a);
	}
	else if (state.path == "login")
	{
		std::cout << "login post method called\n";
		return ErrorResponse::Error_NotFound(a);
	}
	else 
	{
		std::cout << "invalid path for post method \n";
		return ErrorResponse::Error_NotFound(a);
	}
	throw std::runtime_error("Not implemented yet");
	// return response;
}
