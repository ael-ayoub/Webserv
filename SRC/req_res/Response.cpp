#include "../../INCLUDES/Response.hpp"

std::string Response::Display_dir(std::string path, LocationConfig info_location, Config a
    , std::string last_path)
{
    std::string body, s;

    DIR* dir = opendir(path.c_str());
    if (!dir)
        std::cerr << "Error dir\n";

    body += "<html>\n";
    body += "<head><title>Index of ";
    body += info_location.get_path();
    body += "</title></head>\n";
    body += "<link rel=\"icon\" href=\"/favicon.ico\">";
    body += "<body>\n";
    body += "<h1>Index of Dir</h1>\n";
    body += "<ul>\n";
    struct dirent* entry;
    while ((entry = readdir(dir)))
    {
        if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
        {
            body += "    <li><a href=\"" + std::string(entry->d_name) + "\">";
            body += std::string(entry->d_name) + "</a></li>\n";
        }
    }
    body += "None</a></li>\n";
    body += "</ul>\n";
    body += "</body>\n";
    body += "</html>\n";
    closedir(dir);
    std::ostringstream oss;
    oss << body.size();
    s = oss.str();
    std::string header =
	    "HTTP/1.1 200 OK\r\n";
		"Content-Type: " + a.get_mine(last_path) + "\r\n"
		"Content-Length: " + s + "\r\n"
		"Date: Sun, 10 Aug 2025 16:45:00 GMT\r\n"
		"Server: webserv42/1.0\r\n"
		"Connection: close\r\n";

	std::string response;
	response += header;
	response += "\r\n";
	response += body;
    return response;
}

std::string Response::Display_file(std::string last_path, Config a, Request test_request)
{
    std::string body, line, s;
    std::ifstream file((last_path).c_str() , std::ios::in);
	if (!file.is_open())
		return test_request.response_error(a, last_path);
	while (getline(file, line))
		body += line + "\n";
	std::stringstream ss ;
	ss << body.size();
	s = ss.str();
    std::string header =
	    "HTTP/1.1 200 OK\r\n";
	    "Content-Type: " + a.get_mine(last_path) + "\r\n"
	    "Content-Length: " + s + "\r\n"
	    "Date: Sun, 10 Aug 2025 16:45:00 GMT\r\n"
	    "Server: webserv42/1.0\r\n"
	    "Connection: close\r\n";
    std::string response;
	response += header;
	response += "\r\n";
	response += body;
    return response;
}