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
    else if (status_code == "500")
        headers += " Internal Server Error\r\n";
    else if (status_code == "408")
        headers += " Request Timeout\r\n";
    else if (status_code == "504")
        headers += " Gateway Timeout\r\n";
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
    if (status_code == "408")
        body += "  <h1>408 Request Timeout</h1>\n";
    if (status_code == "500")
        body += "  <h1>500 Internal Server Error</h1>\n";
    if (status_code == "504")
        body += "  <h1>504 Gateway Timeout</h1>\n";

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
    if (status_code == "408")
        body += "  <p>The server timed out waiting for the request body. Content-Length may exceed the actual data sent.</p>\n";
    if (status_code == "500")
        body += "  <p>The server encountered an internal error.</p>\n";
    if (status_code == "504")
        body += "  <p>The upstream CGI script did not respond in time.</p>\n";
        
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

std::string ErrorResponse::generate_error_page(std::string status_code)
{
    std::string code = status_code.substr(0, 3);
    std::string message;
    std::string description;
    
    if (code == "400")
    {
        message = "Bad Request";
        description = "Your browser sent a request that this server could not understand.";
    }
    else if (code == "403")
    {
        message = "Forbidden";
        description = "You don't have permission to access this resource.";
    }
    else if (code == "404")
    {
        message = "Not Found";
        description = "The requested resource could not be found on this server.";
    }
    else if (code == "405")
    {
        message = "Method Not Allowed";
        description = "The requested method is not supported for this resource.";
    }
    else if (code == "408")
    {
        message = "Request Timeout";
        description = "The server timed out waiting for the complete request body. The Content-Length header may not match the actual data sent.";
    }
    else if (code == "413")
    {
        message = "Payload Too Large";
        description = "The size of the request entity exceeds the maximum allowed limit.";
    }
    else if (code == "500")
    {
        message = "Internal Server Error";
        description = "The server encountered an internal error.";
    }
    else if (code == "504")
    {
        message = "Gateway Timeout";
        description = "The upstream CGI script did not respond in time.";
    }
    else
    {
        message = "Error";
        description = "An error occurred while processing your request.";
    }
    
    std::string body = 
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <title>ERROR " + code + " - WEBSERV</title>\n"
        "    <link rel=\"stylesheet\" href=\"/style.css\">\n"
        "</head>\n"
        "<body>\n"
        "    <header>\n"
        "        <div class=\"title-block\">\n"
        "            <h1 class=\"brutalist-title\">ERROR</h1>\n"
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
        "            <h1>" + code + " - " + message + "</h1>\n"
        "            <p style=\"margin-top: 1rem; color: var(--text-secondary);\">AN ERROR OCCURRED</p>\n"
        "        </section>\n"
        "\n"
        "        <section class=\"content-section\">\n"
        "            <div class=\"brutalist-box\" style=\"background: #f8d7da; border-color: #dc3545;\">\n"
        "                <span class=\"box-number\" style=\"background: #dc3545; color: white;\">✕</span>\n"
        "                <h2>" + message + "</h2>\n"
        "                <p style=\"margin-top: 1rem; font-size: 1.1rem;\">" + description + "</p>\n"
        "                <div style=\"margin-top: 2rem;\">\n"
        "                    <a href=\"/index.html\" class=\"brutalist-link\">BACK TO HOME →</a>\n"
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
    
    std::stringstream ss;
    ss << body.size();
    std::string s = ss.str();
    
    std::string headers = "HTTP/1.1 ";
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
    else if (status_code == "408")
        headers += " Request Timeout\r\n";
    else if (status_code == "413")
        headers += " Payload Too Large\r\n";
    else if (status_code == "500")
        headers += " Internal Server Error\r\n";
    else if (status_code == "504")
        headers += " Gateway Timeout\r\n";
    
    headers += "Content-Type: text/html\r\n";
    headers += "Content-Length: " + s + "\r\n";
    headers += "Connection: close\r\n";
    headers += "\r\n";
    
    return headers + body;
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

    bool BreakLoop = false;
    while (i < error.size())
    {
        std::map<int, std::string>::iterator it;
        for (it = error[i].begin(); it != error[i].end(); ++it) 
        {
            // std::cout << "status code is : " << status_code << ", we found is : " << it->first << std::endl;
            err_tmp[it->first] = it->second;
            if (it->first == status_code)
            {
                ss << it->first;
                error_status = ss.str();
                path = get_current_path() + it->second;
                BreakLoop = true;
                break;
            }
        }
        if (BreakLoop == true)
            break;
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
    else if (error_status == "408")
        header += " Request Timeout\r\n";
    else if (error_status == "413")
        header += " Payload Too Large\r\n";
    else if (error_status == "500")
        header += " Internal Server Error\r\n";
    else if (error_status == "504")
        header += " Gateway Timeout\r\n";
    // path = err_tmp->second;
    return header;
}

std::string ErrorResponse::Error_GatewayTimeout(Config &a)
{
    (void)a;
    return generate_error_page("504");
}

std::string ErrorResponse::Error_MethodeNotAllowed(Config a)
{
    (void)a;
    return generate_error_page("405");
}

// std::string get_current_path()
// {
//     char buffer[PATH_MAX];
//     if (getcwd(buffer, sizeof(buffer)) != NULL)
//         return std::string(buffer);
//     return "";
// }


std::string ErrorResponse::Responde(Config &a, std::string path, std::string &head, std::string status)
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

std::string ErrorResponse::Error_NotFound(Config &a)
{
    (void)a;
    return generate_error_page("404");
}

std::string ErrorResponse::Error_Internal_Server(Config &a)
{
    (void)a;
    return generate_error_page("500");
}

std::string ErrorResponse::Error_BadRequest(Config &a)
{
    (void)a;
    return generate_error_page("400");
}

std::string ErrorResponse::Error_PayloadTooLarge(Config &a)
{
    (void)a;
    return generate_error_page("413");
}

std::string ErrorResponse::Error_Forbidden(Config &a)
{
    (void)a;
    return generate_error_page("403");
}

std::string ErrorResponse::Error_RequestTimeout(Config &a)
{
    (void)a;
    return generate_error_page("408");
}

std::string ErrorResponse::Error_InternalServerError()
{
    return default_response_error("500");
}