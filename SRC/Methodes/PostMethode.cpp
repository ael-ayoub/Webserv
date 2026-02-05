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
// static std::string ft_getline(int fd)
// {
// 	if (fd < 0)
// 		return "";

// 	char ch;
// 	int byte_read;
// 	std::string line;
// 	while ((byte_read = read(fd, &ch, 1)) > 0)
// 	{
// 		if (byte_read < 0)
// 		{
// 			std::cerr << "Error in read: " << strerror(errno) << std::endl;
// 			return "";
// 		}
// 		if (ch == '\n')
// 			break;
// 		line += ch;
// 	}
// 	return line;
// }

void Upload_files(ClientState &state, const int &fd_client, Config &a)
{
	if (fd_client < 0)
	{
		std::cerr << "Error: Invalid client file descriptor" << std::endl;
		state.response = ErrorResponse::Error_InternalServerError();
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		state.waiting = false;
		throw(std::runtime_error("Invalid client file descriptor"));
	}
	if (state.filename.empty())
	{
		std::cerr << "Error: No filename specified in upload" << std::endl;
		state.response = ErrorResponse::Error_Forbidden(a);
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		state.waiting = false;
		return;
	}
	if (state.boundary.empty())
	{
		std::cerr << "Error: No boundary specified in upload" << std::endl;
		state.response = ErrorResponse::Error_Forbidden(a);
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		state.waiting = false;
		return;
	}
	if (state.content_length <= 0)
	{
		std::cerr << "Error: Content-Length is zero in upload" << std::endl;
		state.response = ErrorResponse::Error_Forbidden(a);
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		state.waiting = false;
		return;
	}
	state.filename_upload = "STATIC/upload/" + state.filename;
	if (state.fd_upload == -1)
	{
		std::cout << "Creating upload file: " << state.filename_upload << std::endl;
		state.fd_upload = open(state.filename_upload.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
	}

	if (state.fd_upload < 0)
	{
		std::cerr << "Error: Unable to create or open file '" << state.filename_upload << "': "
				  << strerror(errno) << std::endl;
		state.response = ErrorResponse::Error_InternalServerError();
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		state.waiting = false;
		throw(std::runtime_error("Unable to create or open upload file"));
	}

	char buffer[4096];
	std::vector<char> tail;
	std::string to_write;

	while (true)
	{
		if(!check_timeout(state.timestamp, TIMEOUT))
		{
			std::cout << "Connection timed out for fd: " << fd_client << std::endl;
			state.response = ErrorResponse::Error_BadRequest(a);
			cloce_connection(state);
			return;
		}
		state.timestamp = get_current_timestamp();
		if (state.readstring.find(state.end_boundary) != std::string::npos)
		{
			size_t end = state.readstring.find(state.end_boundary);
			// size_t size_to_write =
			std::string to_write = state.readstring.substr(0, end);
			if (write(state.fd_upload, to_write.c_str(), to_write.size()) == -1)
			{
				// genertae error
				// remove file
				std::cerr << "Error: Failed to write to file '" << state.filename_upload << "': "
						  << strerror(errno) << std::endl;
				close(state.fd_upload);
				state.response = ErrorResponse::Error_InternalServerError();
				state.close = true;
				state.cleanup = true;
				state.send_data = true;
				state.waiting = false;
				remove(state.filename_upload.c_str());
				return;
			}
			break;
		}
		else if (state.readstring.size() > 0)
		{
			if (state.readstring.find(state.end_boundary) != std::string::npos)
			{
				size_t end = state.readstring.find(state.end_boundary);
				to_write = state.readstring.substr(0, end);
			}
			else
			{
				to_write = state.readstring;
			}
			if (write(state.fd_upload, to_write.c_str(), to_write.size()) == -1)
			{

				std::cerr << "Error: Failed to write to file '" << state.filename_upload << "': "
						  << strerror(errno) << std::endl;
				close(state.fd_upload);
				state.response = ErrorResponse::Error_InternalServerError();
				state.close = true;
				state.cleanup = true;
				state.send_data = true;
				state.waiting = false;
				remove(state.filename_upload.c_str());
				return;
			}
			state.byte_uploaded += to_write.size();
			state.readstring.clear();
			to_write.clear();
		}
		if (state.byte_uploaded - state.metadata.size() >= state.content_length)
		{
			// finished upload
			state.waiting = false;
			buffer[0] = 0;
			break;
		}
		else
		{
			size_t n = read(fd_client, buffer, sizeof(buffer));

			if (n == 0)
			{
				// the cleint close the connection
				std::cerr << "Error: Client closed connection during file upload" << std::endl;
				close(state.fd_upload);
				state.response = ErrorResponse::Error_BadRequest(a);
				state.close = true;
				state.cleanup = true;
				state.send_data = true;
				state.waiting = false;
				remove(state.filename_upload.c_str());
				return;
			}
			else if (n < 0)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
				{
					// should wait for more data
					state.waiting = true;
					return;
				}
				// error happen on the server close the connection
				std::cerr << "Error: Failed to read from client during file upload: "
						  << strerror(errno) << std::endl;
				close(state.fd_upload);
				state.response = ErrorResponse::Error_InternalServerError();
				state.close = true;
				state.cleanup = true;
				state.send_data = true;
				state.waiting = false;
				remove(state.filename_upload.c_str());
				return;
			}
			else
			{
				state.readstring.append(buffer, n);
				state.waiting = false;
			}
		}
		state.timestamp = get_current_timestamp();
	}
	close(state.fd_upload);
	state.fd_upload = -1;
	std::cout << "File upload completed: " << state.filename_upload << std::endl;
	state.response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 20\r\n";
	if (state.header.find("Connection: close") != std::string::npos)
		state.response += "Connection: close\r\n";
	else
		state.response += "Connection: keep-alive\r\n";
	state.response += "\r\nFile uploaded successfully.";
}

std::string get_username_from_metadata(const std::string &metadata)
{
	std::string username_key = "username=";
	size_t pos = metadata.find(username_key);
	if (pos != std::string::npos)
	{
		pos += username_key.length();
		size_t end_pos = metadata.find("\r\n", pos);
		if (end_pos != std::string::npos)
		{
			return metadata.substr(pos, end_pos - pos);
		}
		else
		{
			return metadata.substr(pos);
		}
	}
	return "";
}

void _handle_post_check_user(ClientState &state, Config&a)
{
	std::string username = get_username_from_metadata(state.metadata);
	if (username.empty())
	{
		std::cout << "No username found in metadata" << std::endl;
		state.response = ErrorResponse::Error_BadRequest(a);
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		return;
	}

	if (username == state.cookies)
	{
		std::string body = "User '" + username + "' is logged in.";
		std::stringstream ss;
		ss << body.size();
		std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: " +
			ss.str() + "\r\n";
		if (state.header.find("Connection: close") != std::string::npos)
			response += "Connection: close\r\n";
		else
			response += "Connection: keep-alive\r\n";
		response += "\r\n" + body;
		state.response = response;
	}
	else
	{
		std::string body = "User '" + username + "' is not logged in.";
		std::stringstream ss;
		ss << body.size();
		std::string response =
			"HTTP/1.1 401 Unauthorized\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: " +
			ss.str() + "\r\n";
		if (state.header.find("Connection: close") != std::string::npos)
			response += "Connection: close\r\n";
		else
			response += "Connection: keep-alive\r\n";
		response += "\r\n" + body;
		state.response = response;
	}
	state.close = true;
	state.cleanup = true;
	state.send_data = true;
}


void _handle_post_login(ClientState &state, Config &a)
{
	// Extract username from metadata
	std::string username = get_username_from_metadata(state.metadata);
	if (username.empty())
	{
		std::cout << "No username found in metadata" << std::endl;
		state.response = ErrorResponse::Error_BadRequest(a);
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		return;
	}

	std::string body = "username \'" + username + "\' logged in successfully.";
	std::stringstream ss;
	ss << body.size();
	std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: " +
		ss.str() + "\r\n"
				   "Set-Cookie: username=" +
		username + "; HttpOnly\r\n";
	if (state.header.find("Connection: close") != std::string::npos)
		response += "Connection: close\r\n";
	else
		response += "Connection: keep-alive\r\n";
	response += "\r\n" + body;

	state.response = response;
	state.close = true;
	state.cleanup = true;
	state.send_data = true;

	std::cout << "User '" << username << "' logged in with session ID: " << state.cookies << std::endl;
}

std::string _get_filename(const std::string &metadata)
{
	std::string filename_key = "filename=\"";
	size_t pos = metadata.find(filename_key);
	if (pos != std::string::npos)
	{
		pos += filename_key.length();
		size_t end_quote = metadata.find("\"", pos);
		if (end_quote != std::string::npos)
		{
			return metadata.substr(pos, end_quote - pos);
		}
	}
	return "default_upload_" + generat_random_id();
}

std::string Methodes::PostMethod(Config &a, const int &fd_client, ClientState &state)
{
	if (state.path == "/uploads")
	{
		std::cout << "------------ Handling /uploads POST request -----------" << std::endl;
		if (state.content_type != "multipart/form-data" || state.boundary.empty() || state.metadata.empty())
		{
			state.response = ErrorResponse::Error_BadRequest(a);
			state.close = true;
			state.cleanup = true;
			state.send_data = true;
			return state.response;
		}
		else if (!state.complete_upload)
		{
			state.filename = _get_filename(state.metadata);
			try
			{
				state.timestamp = get_current_timestamp();	
				Upload_files(state, fd_client, a);
			}
			catch (const std::exception &e)
			{
				std::cerr << e.what() << '\n';
				state.response = ErrorResponse::Error_PayloadTooLarge(a);
				state.close = true;
				state.cleanup = true;
				state.send_data = true;
				return state.response;
			}

			state.complete_upload = true;
		}
		state.send_data = true;
		state.cleanup = true;
		if (state.header.find("Connection: close") != std::string::npos)
			state.close = true;
		return state.response;
	}
	else if (state.path == "/login")
	{
		_handle_post_login(state, a);
		return state.response;
	}
	else if (state.path == "/check_user")
	{
		_handle_post_check_user(state,a);
		 return state.response;
	}
	else
	{
		std::cout << "this the correct path: " << state.path << std::endl;
		state.response = ErrorResponse::Error_NotFound(a);
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		return state.response;
	}
}
