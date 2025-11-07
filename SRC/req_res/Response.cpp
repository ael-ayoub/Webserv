#include "../../INCLUDES/Response.hpp"

std::string Response::Display_dir(std::string path, LocationConfig info_location)
{
    std::string body, s;

    DIR* dir = opendir(path.c_str());
    if (!dir)
        std::cerr << "Error dir\n";

    body += "<html>\n";
    body += "<head><title>Index of ";
    body += info_location.get_path();
    body += "</title></head>\n";
    body += "<link rel=\"icon\" href=\"/favicon.ico\">\n";
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
    body += "</ul>\n";
    body += "</body>\n";
    body += "</html>\n";
    closedir(dir);
    std::ostringstream oss;
    oss << body.size();
    s = oss.str();
    std::string header;
    header += "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: text/html\r\n";
	header += "Content-Length: " + s + "\r\n";
	header += "Connection: close\r\n";

	std::string response;
	response += header;
	response += "\r\n";
	response += body;
    return response;
}

std::string Response::Display_file(std::string last_path, Config a)
{
    std::string body, line, s;
    std::ifstream file((last_path).c_str() , std::ios::in);
	if (!file.is_open())
    {
        if (errno == EACCES)
            return ErrorResponse::Error_Forbidden(a);
        else if (errno == ENONET)
		    return ErrorResponse::Error_NotFound(a);
    }
	while (getline(file, line))
		body += line + "\n";
	std::stringstream ss ;
	ss << body.size();
	s = ss.str();
    std::string header;
	header += "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: " + a.get_mine(last_path) + "\r\n";
	header += "Content-Length: " + s + "\r\n";
	header += "Connection: close\r\n";
    std::string response;
	response += header;
	response += "\r\n";
	response += body;
    return response;
}

std::string Response::Get_response(std::string path, LocationConfig info_location,
                                    Request test_request, Config a)
{
    std::string last_path;
    struct stat statbuf;
    std::string pathh = test_request.get_path();
    //std::cout << pathh << std::endl;
    if ( pathh == "/login/" || pathh == "/login/login.html" || pathh == "/login/register.html")
    {
        //std::map<std::string, std::string>::iterator s = test_request.get_session().begin();
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
                return response;
        }
    }
    // std::cout << "path is : " << path << std::endl;
    if (stat(path.c_str(), &statbuf) == 0)
    {
        if (S_ISDIR(statbuf.st_mode))
        {
            if (info_location.get_pathIndex() != "None")
            {
                last_path = info_location.get_root() + test_request.get_path() + info_location.get_pathIndex();
                // std::cout << "last path is : " << last_path << std::endl;
                return Response::Display_file(last_path, a);
            }
            else
            {
                if (info_location.get_autoIndex() == false && info_location.get_pathIndex() == "None")
                    return ErrorResponse::Error_Forbidden(a);
                return Response::Display_dir(path, info_location);
            }
        }
        else
        {
            last_path = info_location.get_root() + test_request.get_path();
            // std::cout << "pathh: " << last_path << std::endl;
            return Response::Display_file(last_path, a);
        }
    }
    return ErrorResponse::Error_NotFound(a);
}

std::string Response::Get_delete(std::string path, LocationConfig info_location,
                                    Request test_request, Config a)
{
    (void)info_location, (void)test_request, (void)a;
    int status = remove(path.c_str());
    if (status != 0)
    {
        if (errno == ENOTEMPTY)
            return "HTTP/1.1 403 Forbidden\r\nConnection: close\r\n\r\n";
        else if (errno == EACCES)
            return "HTTP/1.1 403 Forbidden\r\nConnection: close\r\n\r\n";
        else if (errno == ENOENT)
            return "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        else
            return "HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\n\r\n";
    }
    return "HTTP/1.1 204 No Content\r\nConnection: close\r\n\r\n";
}