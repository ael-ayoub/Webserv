#include "../../INCLUDES/Request.hpp"

std::string Request::default_response_error(Config a)
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
    std::string header =
		"Content-Type: text/html\r\n"
		"Content-Length: " + s + "\r\n"
		"Date: Sun, 10 Aug 2025 16:45:00 GMT\r\n"
		"Server: webserv42/1.0\r\n"
		"Connection: close\r\n";
	response += headers;
	response += header;
	response += "\r\n";
	response += body;
    return response;
}

std::string Request::response_error(Config a, std::string last)
{
    ServerConfig tmp = a.get_server_config();
    std::vector<std::map<int, std::string> > error = tmp.get_error_status();
    std::string body, line, headers, response, s;
    std::map<int, std::string>::iterator err_tmp = error[0].begin();
    
    std::ifstream file((err_tmp->second).c_str() , std::ios::in);
	if (!file.is_open())
        return default_response_error(a);
    headers = "HTTP/1.1 ";
    headers += err_tmp->first;
    headers += " OK\r\n";
	while (getline(file, line))
		body += line + "\n";
	std::stringstream ss ;
	ss << body.size();
	s = ss.str();
    std::string header =
		"Content-Type: " + a.get_mine(last) + "\r\n"
		"Content-Length: " + s + "\r\n"
		"Date: Sun, 10 Aug 2025 16:45:00 GMT\r\n"
		"Server: webserv42/1.0\r\n"
		"Connection: close\r\n";
	response += headers;
	response += header;
	response += "\r\n";
	response += body;
    return response;
}

void Request::parse_request(char *buffer)
{
    int i = 0;
    std::string str(buffer);
    std::string tmp_str;
    while (str[i])
    {
        tmp_str += str[i];
        if (str[i] == '\n')
            break;
        i++;
    }
    Vector_str tmp = ServerConfig::ft_splitv2(tmp_str, ' ');
    size_t j = 0;
    while (j < tmp.size())
    {
        if (j == 0)
            method = tmp[0];
        else if (j == 1)
            path = tmp[1];
        j++;
    }
}

std::string Request::get_method()
{
    return method;
}

std::string Request::get_path()
{
    return path;
}
