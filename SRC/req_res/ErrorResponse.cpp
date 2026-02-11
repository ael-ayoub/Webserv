#include "../../INCLUDES/ErrorResponse.hpp"

std::string ErrorResponse::default_response_error(std::string status_code)
{
    std::string body, headers, response, s;

    headers = "HTTP/1.1 ";
    headers += status_code;
    if (status_code == "200")
        headers += " OK\r\n";
    else if (status_code == "204")
        headers += " No Content\r\n";
    else if (status_code == "400")
        headers += " Bad Request\r\n";
    else if (status_code == "403")
        headers += " Forbidden\r\n";
    else if (status_code == "404")
        headers += " Not Found\r\n";
    else if (status_code == "405")
        headers += " Method Not Allowed\r\n";
    else if (status_code == "413")
        headers += " Payload Too Large\r\n";
    body = "<!DOCTYPE html>\n";
    body += "<html>\n";
    body += "<head>\n";
    body += "  <meta charset=\"UTF-8\">\n";
    body += "  <title>Error</title>\n";
    body += "</head>\n";
    body += "<body>\n";

    if (status_code == "404")
        body += "  <h1>404 Not Found</h1>\n";
    if (status_code == "405")
        body += "  <h1>405 Method Not Allowed</h1>\n";
    if (status_code == "400")
        body += "  <h1>400 Bad Request</h1>\n";
    if (status_code == "403")
        body += "  <h1>403 Forbidden</h1>\n";
    if (status_code == "413")
        body += "  <h1>413 Payload Too Large</h1>\n";

    if (status_code == "404")
        body += "  <p>The requested resource could not be found on this server.</p>\n";
    if (status_code == "405")
        body += "  <p>The requested method is not supported for this resource.</p>\n";
    if (status_code == "400")
        body += "  <p>Your browser sent a request that this server could not understand.</p>\n";
    if (status_code == "403")
        body += "  <p>You don’t have permission to access this resource.</p>\n";
    if (status_code == "413")
        body += "  <p>The size of the request entity exceeds the maximum allowed limit.</p>\n";
        
    body += "</body>\n";
    body += "</html>\n";
	std::stringstream ss ;
	ss << body.size();
	s = ss.str();
    std::string header;
	header += "Content-Type: text/html\r\n";
	header += "Content-Length: " + s + "\r\n";
	header += "Connection: close\r\n";
	response += headers;
	response += header;
	response += "\r\n";
	response += body;
    return response;
}

// std::string ErrorResponse::response_error(Config a, std::string last)
// {
//     ServerConfig tmp = a.get_server_config();
//     std::vector<std::map<int, std::string> > error = tmp.get_error_status();
//     std::string body, line, header, response, s;
//     std::map<int, std::string>::iterator err_tmp = error[0].begin();
    
//     std::ifstream file((err_tmp->second).c_str() , std::ios::in);
// 	if (!file.is_open())
//         return default_response_error(a);
// 	while (getline(file, line))
// 		body += line + "\n";
// 	std::stringstream ss ;
// 	ss << body.size();
// 	s = ss.str();

//     std::string error_status;
//     std::stringstream sss;
//     sss << err_tmp->first;
//     error_status = sss.str();
//     header = "HTTP/1.1 ";
//     header += error_status;
//     header += " OK\r\n";
// 	header += "Content-Type: " + a.get_mine(last) + "\r\n";
// 	header += "Content-Length: " + s + "\r\n";
// 	header += "Connection: close\r\n";

// 	response += header;
// 	response += "\r\n";
// 	response += body;
//     return response;
// }

std::string ErrorResponse::check_errorstatus(std::vector<std::map<int, std::string> > error,
                                            int status_code, std::string& path)
{
    size_t i = 0;
    std::map<int, std::string> err_tmp;
    std::string error_status;
    std::stringstream ss;

    while (i < error.size())
    {
        std::map<int, std::string>::iterator it;
        for (it = error[i].begin(); it != error[i].end(); ++it) 
        {
            err_tmp[it->first] = it->second;
            if (it->first == status_code)
            {
                ss << it->first;
                error_status = ss.str();
                path = it->second;
                break;
            }
        }
        i++;
    }
    // int key = err_tmp.begin();
    // ss << err_tmp.begin();
    // error_status = ss.str();
    if (i == error.size())
        return "";
    std::string header;
    header = "HTTP/1.1 ";
    header += error_status;
    // std::cout << "err: " << error_status << std::endl;
    if (error_status == "200")
        header += " OK\r\n";
    else if (error_status == "204")
        header += " No Content\r\n";
    else if (error_status == "400")
        header += " Bad Request\r\n";
    else if (error_status == "403")
        header += " Forbidden\r\n";
    else if (error_status == "404")
        header += " Not Found\r\n";
    else if (error_status == "405")
        header += " Method Not Allowed\r\n";
    // path = err_tmp->second;
    return header;
}

std::string ErrorResponse::Error_MethodeNotAllowed(Config a)
{
    std::string path, line, body, s;

    ServerConfig tmp = a.get_server_config();

    std::vector<std::map<int, std::string> > error = tmp.get_error_status();

    std::string header = check_errorstatus(error, 405, path);

    if (header.empty() == true)
        return default_response_error("405");
        
    header += Responde(a, path, header, "405");
    return header;
}

std::string ErrorResponse::Responde(Config a, std::string path, std::string &head, std::string status)
{
    std::string body, header, s, line;
    std::ifstream file((path).c_str() , std::ios::in);
	if (!file.is_open())
    {
        head = "";
        return default_response_error(status);
    }

	while (getline(file, line))
		body += line + "\n";

	std::stringstream ss;
	ss << body.size();
	s = ss.str();

    std::string response;
	header += "Content-Type: " + a.get_mine(path) + "\r\n";
	header += "Content-Length: " + s + "\r\n";
	header += "Connection: close\r\n";
	header += "\r\n";

	response += header;
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
        return default_response_error("404");
        
    header += Responde(a, path, header, "404");
    return header;
}

std::string ErrorResponse::Error_BadRequest(Config &a)
{
    std::string path, line, body, s;

    ServerConfig tmp = a.get_server_config();

    std::vector<std::map<int, std::string> > error = tmp.get_error_status();

    std::string header = check_errorstatus(error, 400, path);

    if (header.empty() == true)
        return default_response_error("400");
        
    header += Responde(a, path, header, "400");
    return header;
}

std::string ErrorResponse::Error_PayloadTooLarge(Config &a)
{
    std::string path, line, body, s;

    ServerConfig tmp = a.get_server_config();

    std::vector<std::map<int, std::string> > error = tmp.get_error_status();

    std::string header = check_errorstatus(error, 400, path);

    if (header.empty() == true)
        return default_response_error("413");
        
    header += Responde(a, path, header, "413");
    return header;
}

std::string ErrorResponse::Error_Forbidden(Config a)
{
    std::string path, line, body, s;

    ServerConfig tmp = a.get_server_config();

    std::vector<std::map<int, std::string> > error = tmp.get_error_status();

    std::string header = check_errorstatus(error, 403, path);

    if (header.empty() == true)
        return default_response_error("403");
        
    header += Responde(a, path, header, "403");
    return header;
}