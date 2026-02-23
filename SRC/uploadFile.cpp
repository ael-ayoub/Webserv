#include "../INCLUDES/Webserv.hpp"

bool _uploadFile(const int &fd_client, ClientState &state)
{

	if (fd_client < 0)
	{
		std::cerr << "Error: Invalid client file descriptor" << std::endl;
		return false;
	}
	std::string &header = state.header;
	std::string boundary;
	size_t pos = header.find("boundary=");
	if (pos != std::string::npos)
	{
		pos += 9;
		size_t end = header.find("\r", pos);
		if (end != std::string::npos)
			boundary = header.substr(pos, end - pos);
	}
	std::string metadata = state.metadata;

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
		return false;
	}

	std::string u_path = "www/upload/" + filename;

	if (state.fd_upload < 0)
	{
		state.fd_upload = open(u_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
		if (state.fd_upload < 0)
		{

			std::string error_msg = "Error: Failed to open/create file '" + u_path + "': " + strerror(errno);
			throw std::runtime_error(error_msg);
		}
	}
	std::string end_boundary = "\r\n--"+ boundary + "--";

	char buffer[4096];
	std::vector<char> &tail = state.tail;
	bool found = false;
	while (!found)
	{
		ssize_t byte_read = read(fd_client, buffer, sizeof(buffer));
		if (byte_read == 0)
		{
			// std::cout << "upload done " << std::endl;
			return true;
		}
		if (byte_read < 0)
		{
			if (errno == EWOULDBLOCK)
				return false;
			else
			{
				// std::cout << "read function failed" << std::endl;
				close(state.fd_upload);
				state.fd_upload = -1;
				return false;
			}
		}

		std::vector<char> check;
		check.reserve(tail.size() + byte_read);
		check.insert(check.end(), tail.begin(), tail.end());
		check.insert(check.end(), buffer, buffer + byte_read);

		std::string check_str(check.begin(), check.end());
		size_t pos = check_str.find(end_boundary);
		if (pos != std::string::npos)
		{
			if (write(state.fd_upload, check.data(), pos) == -1)
			{
				std::cerr << "Error: Failed to write to file '" << u_path << "': "
						  << strerror(errno) << std::endl;
				return false;
			}
			state.complete_upload = true;
			return true;
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
			if (write(state.fd_upload, check.data(), write_size) == -1)
			{
				std::cerr << "Error: Failed to write to file '" << u_path << "': "
						  << strerror(errno) << std::endl;
				return false;
			}
		}
	}
	return true;
}