#include "../../INCLUDES/Webserv.hpp"

#include "../../INCLUDES/CGI.hpp"

#include <sstream>

std::string generat_random_id();

static std::string _trim(const std::string &s)
{
	size_t start = 0;
	while (start < s.size() && (s[start] == ' ' || s[start] == '\t'))
		start++;
	size_t end = s.size();
	while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n'))
		end--;
	return s.substr(start, end - start);
}

static std::string _header_value(const std::string &header, const std::string &key)
{
	const std::string needle = key + ":";
	size_t pos = header.find(needle);
	if (pos == std::string::npos)
		return "";
	pos += needle.size();
	while (pos < header.size() && (header[pos] == ' ' || header[pos] == '\t'))
		pos++;
	size_t end = header.find("\r\n", pos);
	if (end == std::string::npos)
		end = header.size();
	return _trim(header.substr(pos, end - pos));
}

static std::string _itoa(int n)
{
	std::ostringstream oss;
	oss << n;
	return oss.str();
}

static std::string _url_decode_simple(const std::string &in)
{
	std::string out;
	out.reserve(in.size());
	for (size_t i = 0; i < in.size(); i++)
	{
		if (in[i] == '%' && i + 2 < in.size())
		{
			char hex[3];
			hex[0] = in[i + 1];
			hex[1] = in[i + 2];
			hex[2] = 0;
			char *end = NULL;
			long v = strtol(hex, &end, 16);
			if (end && *end == 0)
			{
				out += static_cast<char>(v);
				i += 2;
				continue;
			}
		}
		if (in[i] == '+')
			out += ' ';
		else
			out += in[i];
	}
	return out;
}

static std::string _cwd()
{
	char buffer[PATH_MAX];
	if (getcwd(buffer, sizeof(buffer)) != NULL)
		return std::string(buffer);
	return "";
}

static std::string _get_filename_from_headers_or_query(const ClientState &state)
{
	// 1) Query string: /uploads?filename=...
	std::string path = state.path;
	size_t qpos = path.find('?');
	if (qpos == std::string::npos)
		qpos = path.find("%3F");
	if (qpos != std::string::npos)
	{
		std::string qs = (path[qpos] == '?') ? path.substr(qpos + 1) : path.substr(qpos + 3);
		std::string key = "filename=";
		size_t p = qs.find(key);
		if (p != std::string::npos)
		{
			p += key.size();
			size_t amp = qs.find('&', p);
			std::string val = (amp == std::string::npos) ? qs.substr(p) : qs.substr(p, amp - p);
			val = _url_decode_simple(val);
			if (!val.empty())
				return val;
		}
	}

	// 2) Custom header: X-Filename: name.ext
	std::string xfn = _header_value(state.header, "X-Filename");
	if (!xfn.empty())
		return xfn;

	// 3) Content-Disposition: ... filename="..."
	std::string cd = _header_value(state.header, "Content-Disposition");
	if (!cd.empty())
	{
		std::string needle = "filename=\"";
		size_t p = cd.find(needle);
		if (p != std::string::npos)
		{
			p += needle.size();
			size_t endq = cd.find('"', p);
			if (endq != std::string::npos)
				return cd.substr(p, endq - p);
		}
	}

	return "default_upload_" + generat_random_id();
}

static bool _is_uploads_path(const std::string &path)
{
	// Accept: /uploads, /uploads?..., /uploads%3f...
	std::string p = path;
	while (!p.empty() && (unsigned char)p[0] <= 32)
		p.erase(0, 1);
	while (!p.empty() && (unsigned char)p[p.size() - 1] <= 32)
		p.erase(p.size() - 1);

	if (p.size() < 8)
		return false;
	if (p.compare(0, 8, "/uploads") != 0)
		return false;
	if (p.size() == 8)
		return true;
	if (p[8] == '?')
		return true;
	if (p.size() >= 11 && p[8] == '%')
	{
		std::string enc = p.substr(8, 3);
		for (size_t i = 0; i < enc.size(); i++)
			enc[i] = (char)std::tolower(enc[i]);
		if (enc == "%3f")
			return true;
	}
	return false;
}

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

static bool Upload_files(ClientState &state, const int &fd_client, Config &a)
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
		return true;
	}
	if (state.boundary.empty())
	{
		std::cerr << "Error: No boundary specified in upload" << std::endl;
		state.response = ErrorResponse::Error_Forbidden(a);
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		state.waiting = false;
		return true;
	}
	if (state.content_length <= 0)
	{
		std::cerr << "Error: Content-Length is zero in upload" << std::endl;
		state.response = ErrorResponse::Error_Forbidden(a);
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		state.waiting = false;
		return true;
	}
	if (state.filename.find("..") != std::string::npos || state.filename.find('/') != std::string::npos)
	{
		state.response = ErrorResponse::Error_Forbidden(a);
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		state.waiting = false;
		return true;
	}
	mkdir("www", 0755);
	mkdir("www/upload", 0755);
	state.filename_upload = "www/upload/" + state.filename;
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
			return true;
		}
		state.timestamp = get_current_timestamp();
		if (state.readstring.find(state.end_boundary) != std::string::npos)
		{
			size_t end = state.readstring.find(state.end_boundary);
			std::string to_write = state.readstring.substr(0, end);
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
				return true;
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
				return true;
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
				return true;
			}
			else if (n < 0)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
				{
					// should wait for more data
					state.waiting = true;
					state.send_data = false;
					return false;
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
				return true;
			}
			else
			{
				try
				{
					state.readstring.append(buffer, n);
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
				
				state.waiting = false;
				state.send_data = false;
			}
		}
		state.timestamp = get_current_timestamp();
	}
	close(state.fd_upload);
	state.fd_upload = -1;
	std::cout << "File upload completed: " << state.filename_upload << std::endl;
	state.complete_upload = true;
	
	// Extract filename from path
	std::string filename = state.filename_upload;
	size_t last_slash = filename.find_last_of('/');
	if (last_slash != std::string::npos)
		filename = filename.substr(last_slash + 1);
	
	std::string body = 
		"<!DOCTYPE html>\n"
		"<html lang=\"en\">\n"
		"<head>\n"
		"    <meta charset=\"UTF-8\">\n"
		"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
		"    <title>UPLOAD SUCCESS - WEBSERV</title>\n"
		"    <link rel=\"stylesheet\" href=\"/style.css\">\n"
		"</head>\n"
		"<body>\n"
		"    <header>\n"
		"        <div class=\"title-block\">\n"
		"            <h1 class=\"brutalist-title\">UPLOAD SUCCESS</h1>\n"
		"        </div>\n"
		"        <nav>\n"
		"            <a href=\"/index.html\">HOME</a>\n"
		"            <a href=\"/about.html\">ABOUT</a>\n"
		"            <a href=\"/upload.html\">UPLOAD</a>\n"
		"            <a href=\"/uploads_manager.html\">UPLOADS</a>\n"
		"            <a href=\"/session.html\">SESSION</a>\n"
		"            <a href=\"/cgi_test.html\">CGI TEST</a>\n"
		"        </nav>\n"
		"    </header>\n"
		"\n"
		"    <main>\n"
		"        <section class=\"page-header\">\n"
		"            <h1>FILE UPLOADED</h1>\n"
		"            <p style=\"margin-top: 1rem; color: var(--text-secondary);\">POST REQUEST COMPLETED SUCCESSFULLY</p>\n"
		"        </section>\n"
		"\n"
		"        <section class=\"content-section\">\n"
		"            <div class=\"brutalist-box\" style=\"background: #d4edda; border-color: #28a745;\">\n"
		"                <span class=\"box-number\" style=\"background: #28a745; color: white;\">✓</span>\n"
		"                <h2>UPLOAD COMPLETED</h2>\n"
		"                <p style=\"margin-top: 1rem; font-size: 1.1rem;\">The following file has been successfully uploaded:</p>\n"
		"                <div style=\"margin-top: 1.5rem; padding: 1rem; background: white; border: 2px solid #000;\">\n"
		"                    <code style=\"font-size: 1.2rem; font-weight: bold;\">" + filename + "</code>\n"
		"                </div>\n"
		"                <div style=\"margin-top: 2rem;\">\n"
		"                    <a href=\"/uploads_manager.html\" class=\"brutalist-link\">VIEW ALL UPLOADS →</a>\n"
		"                    <a href=\"/upload.html\" class=\"brutalist-link\" style=\"margin-left: 1rem;\">UPLOAD ANOTHER FILE →</a>\n"
		"                </div>\n"
		"            </div>\n"
		"        </section>\n"
		"    </main>\n"
		"\n"
		"    <footer>\n"
		"        <p>© 2026 WEBSERV PROJECT | C++98 COMPLIANT</p>\n"
		"    </footer>\n"
		"</body>\n"
		"</html>\n";
	
	std::ostringstream oss;
	oss << body.size();
	state.response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + oss.str() + "\r\n";
	if (state.header.find("Connection: close") != std::string::npos)
		state.response += "Connection: close\r\n";
	else
		state.response += "Connection: keep-alive\r\n";
	state.response += "\r\n" + body;
	state.send_data = true;
	state.cleanup = true;
	state.waiting = false;
	return true;
}

static bool Upload_raw(ClientState &state, const int &fd_client, Config &a)
{
	if (state.filename.empty())
	{
		state.response = ErrorResponse::Error_BadRequest(a);
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		state.waiting = false;
		return true;
	}
	if (state.content_length == 0)
	{
		state.response = ErrorResponse::Error_BadRequest(a);
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		state.waiting = false;
		return true;
	}
	if (state.filename.find("..") != std::string::npos || state.filename.find('/') != std::string::npos)
	{
		state.response = ErrorResponse::Error_Forbidden(a);
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		state.waiting = false;
		return true;
	}

	mkdir("www", 0755);
	mkdir("www/upload", 0755);
	if (state.filename_upload.empty())
		state.filename_upload = "www/upload/" + state.filename;
	if (state.fd_upload == -1)
		state.fd_upload = open(state.filename_upload.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (state.fd_upload < 0)
	{
		state.response = ErrorResponse::Error_InternalServerError();
		state.close = true;
		state.cleanup = true;
		state.send_data = true;
		state.waiting = false;
		return true;
	}

	if (!state.readstring.empty())
	{
		ssize_t w = write(state.fd_upload, state.readstring.c_str(), state.readstring.size());
		if (w < 0)
		{
			close(state.fd_upload);
			state.fd_upload = -1;
			remove(state.filename_upload.c_str());
			state.response = ErrorResponse::Error_InternalServerError();
			state.close = true;
			state.cleanup = true;
			state.send_data = true;
			state.waiting = false;
			return true;
		}
		state.byte_uploaded += (size_t)w;
		state.readstring.erase(0, (size_t)w);
	}

	char buffer[4096];
	while (state.byte_uploaded < state.content_length)
	{
		ssize_t n = read(fd_client, buffer, sizeof(buffer));
		if (n == 0)
		{
			close(state.fd_upload);
			state.fd_upload = -1;
			remove(state.filename_upload.c_str());
			state.response = ErrorResponse::Error_BadRequest(a);
			state.close = true;
			state.cleanup = true;
			state.send_data = true;
			state.waiting = false;
			return true;
		}
		if (n < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				state.waiting = true;
				return false;
			}
			close(state.fd_upload);
			state.fd_upload = -1;
			remove(state.filename_upload.c_str());
			state.response = ErrorResponse::Error_InternalServerError();
			state.close = true;
			state.cleanup = true;
			state.send_data = true;
			state.waiting = false;
			return true;
		}

		size_t remaining = state.content_length - state.byte_uploaded;
		size_t to_write = (remaining < (size_t)n) ? remaining : (size_t)n;
		ssize_t w = write(state.fd_upload, buffer, to_write);
		if (w < 0)
		{
			close(state.fd_upload);
			state.fd_upload = -1;
			remove(state.filename_upload.c_str());
			state.response = ErrorResponse::Error_InternalServerError();
			state.close = true;
			state.cleanup = true;
			state.send_data = true;
			state.waiting = false;
			return true;
		}
		state.byte_uploaded += (size_t)w;
	}

	close(state.fd_upload);
	state.fd_upload = -1;
	state.complete_upload = true;
	const std::string body = "File uploaded successfully.";
	std::ostringstream oss;
	oss << body.size();
	state.response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: " + oss.str() + "\r\n";
	if (state.header.find("Connection: close") != std::string::npos)
		state.response += "Connection: close\r\n";
	else
		state.response += "Connection: keep-alive\r\n";
	state.response += "\r\n" + body;
	state.send_data = true;
	state.cleanup = true;
	state.waiting = false;
	return true;
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
	// CGI for POST (e.g. /upload/script.py)
	{
		std::string req_path = state.path;
		std::string query_string;
		size_t qpos = req_path.find('?');
		if (qpos != std::string::npos)
		{
			query_string = req_path.substr(qpos + 1);
			req_path = req_path.substr(0, qpos);
		}
		LocationConfig info_location = a.get_info_location(req_path);
		if (info_location.get_path() != "None")
		{
			const size_t dot = req_path.rfind('.');
			if (dot != std::string::npos)
			{
				const std::string ext = req_path.substr(dot);
				const std::string binary = info_location.get_cgi_binary(ext);
				if (!binary.empty())
				{
					if (!info_location.get_method("POST"))
					{
						state.response = ErrorResponse::Error_MethodeNotAllowed(a);
						state.close = true;
						state.cleanup = true;
						state.send_data = true;
						state.waiting = false;
						return state.response;
					}

					std::string script_fs = _cwd() + info_location.get_root() + req_path;
					struct stat st;
					if (stat(script_fs.c_str(), &st) != 0)
					{
						state.response = ErrorResponse::Error_NotFound(a);
						state.close = true;
						state.cleanup = true;
						state.send_data = true;
						state.waiting = false;
						return state.response;
					}

					if (state.body_tmp_path.empty())
						state.body_tmp_path = "./SRC/temp/cgi_stdin_" + _itoa(fd_client) + ".tmp";
					if (state.fd_body == -1)
						state.fd_body = open(state.body_tmp_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
					if (state.fd_body < 0)
					{
						state.response = ErrorResponse::Error_InternalServerError();
						state.close = true;
						state.cleanup = true;
						state.send_data = true;
						state.waiting = false;
						return state.response;
					}

					// write any buffered body bytes first
					if (!state.readstring.empty())
					{
						ssize_t w = write(state.fd_body, state.readstring.c_str(), state.readstring.size());
						if (w < 0)
						{
							close(state.fd_body);
							state.fd_body = -1;
							remove(state.body_tmp_path.c_str());
							state.body_tmp_path.clear();
							state.body_received = 0;
							state.response = ErrorResponse::Error_InternalServerError();
							state.close = true;
							state.cleanup = true;
							state.send_data = true;
							state.waiting = false;
							return state.response;
						}
						state.body_received += (size_t)w;
						state.readstring.erase(0, (size_t)w);
					}

					char buf[4096];
					while (state.body_received < state.content_length)
					{
						ssize_t n = read(fd_client, buf, sizeof(buf));
						if (n == 0)
						{
							close(state.fd_body);
							state.fd_body = -1;
							remove(state.body_tmp_path.c_str());
							state.body_tmp_path.clear();
							state.body_received = 0;
							state.response = ErrorResponse::Error_BadRequest(a);
							state.close = true;
							state.cleanup = true;
							state.send_data = true;
							state.waiting = false;
							return state.response;
						}
						if (n < 0)
						{
							if (errno == EAGAIN || errno == EWOULDBLOCK)
							{
								state.waiting = true;
								return state.response;
							}
							close(state.fd_body);
							state.fd_body = -1;
							remove(state.body_tmp_path.c_str());
							state.body_tmp_path.clear();
							state.body_received = 0;
							state.response = ErrorResponse::Error_InternalServerError();
							state.close = true;
							state.cleanup = true;
							state.send_data = true;
							state.waiting = false;
							return state.response;
						}
						size_t remaining = state.content_length - state.body_received;
						size_t to_write = (remaining < (size_t)n) ? remaining : (size_t)n;
						ssize_t w = write(state.fd_body, buf, to_write);
						if (w < 0)
						{
							close(state.fd_body);
							state.fd_body = -1;
							remove(state.body_tmp_path.c_str());
							state.body_tmp_path.clear();
							state.body_received = 0;
							state.response = ErrorResponse::Error_InternalServerError();
							state.close = true;
							state.cleanup = true;
							state.send_data = true;
							state.waiting = false;
							return state.response;
						}
						state.body_received += (size_t)w;
					}

					close(state.fd_body);
					state.fd_body = -1;

					std::vector<std::string> envs;
					envs.push_back("REQUEST_METHOD=POST");
					envs.push_back("QUERY_STRING=" + query_string);
					{
						std::ostringstream oss;
						oss << state.content_length;
						envs.push_back("CONTENT_LENGTH=" + oss.str());
					}
					envs.push_back("CONTENT_TYPE=" + state.raw_content_type);
					envs.push_back("GATEWAY_INTERFACE=CGI/1.1");
					envs.push_back("SERVER_PROTOCOL=HTTP/1.1");
					envs.push_back("SCRIPT_NAME=" + req_path);

					std::vector<const char *> envp;
					for (size_t i = 0; i < envs.size(); i++)
						envp.push_back(envs[i].c_str());
					envp.push_back(NULL);

					CGI cgi("POST", script_fs.c_str(), ext.c_str(), binary.c_str(), &envp[0], state.body_tmp_path.c_str());
					if (cgi.CGIProccess())
						state.response = cgi.response;
					else if (cgi.TimedOut())
						state.response = ErrorResponse::Error_GatewayTimeout(a);
					else
						state.response = ErrorResponse::default_response_error("500");

					remove(state.body_tmp_path.c_str());
					state.body_tmp_path.clear();
					state.body_received = 0;
					state.send_data = true;
					state.cleanup = true;
					state.waiting = false;
					return state.response;
				}
			}
		}
	}

	// Upload endpoint supports multipart/form-data and raw bodies
	if (_is_uploads_path(state.path))
	{
		std::cout << "------------ Handling /uploads POST request -----------" << std::endl;
		// Hard cap: 1 GB — prevents hang when client_max_body_size == 0 (unlimited)
		const size_t _hard_cap = 1024ULL * 1024ULL * 1024ULL;
		if (state.content_length > _hard_cap ||
			(ServerConfig::client_max_body_size != 0 && state.content_length > ServerConfig::client_max_body_size))
		{
			state.response = ErrorResponse::Error_PayloadTooLarge(a);
			state.close = true;
			state.cleanup = true;
			state.send_data = true;
			return state.response;
		}
		if (!state.complete_upload)
		{
			state.timestamp = get_current_timestamp();
			if (state.content_type == "multipart/form-data")
			{
				if (state.boundary.empty() || state.metadata.empty())
				{
					state.response = ErrorResponse::Error_BadRequest(a);
					state.close = true;
					state.cleanup = true;
					state.send_data = true;
					state.waiting = false;
					return state.response;
				}
				state.filename = _get_filename(state.metadata);
				try
				{
					bool done = Upload_files(state, fd_client, a);
					if (!done)
						return state.response;
				}
				catch (const std::exception &e)
				{
					std::cerr << e.what() << '\n';
					state.response = ErrorResponse::Error_InternalServerError();
					state.close = true;
					state.cleanup = true;
					state.send_data = true;
					state.waiting = false;
					return state.response;
				}
			}
			else
			{
				state.filename = _get_filename_from_headers_or_query(state);
				bool done = Upload_raw(state, fd_client, a);
				if (!done)
					return state.response;
			}
		}
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
