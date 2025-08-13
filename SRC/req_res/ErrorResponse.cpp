#include "../../INCLUDES/ErrorResponse.hpp"

std::string ErrorResponse::default_response_error(Config a)
{
    std::string body, line, headers, response, s;
    ServerConfig tmp = a.get_server_config();
    std::vector<std::map<int, std::string> > error = tmp.get_error_status();
    std::map<int, std::string>::iterator err_tmp = error[0].begin();
    headers = "HTTP/1.1 ";
    headers += err_tmp->first;
    headers += " OK\r\n";
    body =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "  <meta charset=\"UTF-8\">\n"
        "  <title>Error 404</title>\n"
        "</head>\n"
        "<body>\n"
        "  <h1> Error </h1>\n"
        "  <p>The requested resource could not be found on this server.</p>\n"
        "</body>\n"
        "</html>\n";
	std::stringstream ss ;
	ss << body.size();
	s = ss.str();
    std::string header;
	header += "Content-Type: text/html\r\n";
	header += "Content-Length: " + s + "\r\n";
	header += "Date: Sun, 10 Aug 2025 16:45:00 GMT\r\n";
	header += "Server: webserv42/1.0\r\n";
	header += "Connection: close\r\n";
	response += headers;
	response += header;
	response += "\r\n";
	response += body;
    return response;
}

std::string ErrorResponse::response_error(Config a, std::string last)
{
    ServerConfig tmp = a.get_server_config();
    std::vector<std::map<int, std::string> > error = tmp.get_error_status();
    std::string body, line, header, response, s;
    std::map<int, std::string>::iterator err_tmp = error[0].begin();
    
    std::ifstream file((err_tmp->second).c_str() , std::ios::in);
	if (!file.is_open())
        return default_response_error(a);
	while (getline(file, line))
		body += line + "\n";
	std::stringstream ss ;
	ss << body.size();
	s = ss.str();

    header = "HTTP/1.1 ";
    header += err_tmp->first;
    header += " OK\r\n";
	header += "Content-Type: " + a.get_mine(last) + "\r\n";
	header += "Content-Length: " + s + "\r\n";
	header += "Connection: close\r\n";

	response += header;
	response += "\r\n";
	response += body;
    return response;
}

std::string ErrorResponse::check_errorstatus(std::vector<std::map<int, std::string> > error,
                                            int status_code, std::string& path)
{
    size_t i = 0;
    std::map<int, std::string>::iterator err_tmp;

    while (i < error.size())
    {
        err_tmp = error[i].begin();
        if (err_tmp->first == status_code)
            break;
        i++;
    }

    if (i == error.size())
        return "";

    std::string header;
    header = "HTTP/1.1 ";
    header += err_tmp->first;
    header += " OK\r\n";
    path = err_tmp->second;
    return header;
}

std::string ErrorResponse::Error_MethodeNotAllowed(Config a)
{
    std::string path, line, body, s;

    ServerConfig tmp = a.get_server_config();

    std::vector<std::map<int, std::string> > error = tmp.get_error_status();

    std::string header = check_errorstatus(error, 405, path);

    if (header.empty() == true)
        return "";
        
    header += Responde(a, path);
    return header;
}

std::string ErrorResponse::Responde(Config a, std::string path)
{
    std::string body, header, s, line;
    std::ifstream file((path).c_str() , std::ios::in);
	if (!file.is_open())
        return default_response_error(a);

	while (getline(file, line))
		body += line + "\n";

	std::stringstream ss;
	ss << body.size();
	s = ss.str();

    std::string response;
	header += "Content-Type: " + a.get_mine(path) + "\r\n";
	header += "Content-Length: " + s + "\r\n";
	header += "Connection: close\r\n";

	response += header;
	response += "\r\n";
	response += body;
    return response;
}

std::string ErrorResponse::Error_NotFound(Config a)
{
    std::string path, line, body, s;

    ServerConfig tmp = a.get_server_config();

    std::vector<std::map<int, std::string> > error = tmp.get_error_status();

    std::string header = check_errorstatus(error, 404, path);

    if (header.empty() == true)
        return "";

    header += Responde(a, path);
    return header;
}
