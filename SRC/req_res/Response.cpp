#include "../../INCLUDES/Response.hpp"
#include "../../INCLUDES/CGI.hpp"

std::string gcwdd()
{
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
        return std::string(buffer);
    return "";
}

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

std::string Response::Get_response(std::string path, LocationConfig &info_location,
                                    Request test_request, Config a)
{
    std::string last_path;
    struct stat statbuf;
    std::string request_path = test_request.get_path();
    std::string query_string;
    size_t qpos = request_path.find('?');
    if (qpos != std::string::npos)
    {
        query_string = request_path.substr(qpos + 1);
        request_path = request_path.substr(0, qpos);
    }
    
    // std::cout << "path is : " << path << std::endl;

    // std::cout << "reach here\n";
    if (info_location.GetRedirectionBool() == true)
    {
        // std::cout << "path is : " << info_location.get_path() << "\n";
        std::string res;
        res += "HTTP/1.1 301 Moved Permanently\r\n";
        res += "Location: " + info_location.GetLocationPath() + "\r\n";
        res += "\r\n";
        return res;
    }
    if (stat(path.c_str(), &statbuf) == 0)
    {
        // std::cout << "before anything here\n";
        if (S_ISDIR(statbuf.st_mode))
        {
            if (info_location.get_pathIndex() != "None")
            {
                last_path = gcwdd() + info_location.get_root() + test_request.get_path() + info_location.get_pathIndex();
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
            last_path = gcwdd() + info_location.get_root() + request_path;
            // std::cout << "pathh: " << last_path << std::endl;

            const size_t dot = last_path.rfind('.');
            if (dot != std::string::npos)
            {
                const std::string ext = last_path.substr(dot);
                const std::string binary = info_location.get_cgi_binary(ext);
                if (!binary.empty())
                {
                    std::vector<std::string> envs;
                    envs.push_back("REQUEST_METHOD=" + test_request.get_method());
                    envs.push_back("QUERY_STRING=" + query_string);
                    {
                        std::ostringstream oss;
                        oss << test_request.get_content_length();
                        envs.push_back("CONTENT_LENGTH=" + oss.str());
                    }
                    envs.push_back("CONTENT_TYPE=");
                    envs.push_back("GATEWAY_INTERFACE=CGI/1.1");
                    envs.push_back("SERVER_PROTOCOL=HTTP/1.1");
                    envs.push_back("SCRIPT_NAME=" + request_path);

                    std::vector<const char *> envp;
                    for (size_t i = 0; i < envs.size(); i++)
                        envp.push_back(envs[i].c_str());
                    envp.push_back(NULL);

                    CGI cgi(test_request.get_method().c_str(), last_path.c_str(), ext.c_str(), binary.c_str(), &envp[0]);
                    if (cgi.CGIProccess())
                        return cgi.response;
					if (cgi.TimedOut())
						return ErrorResponse::Error_GatewayTimeout(a);
					return ErrorResponse::Error_Internal_Server(a);
                }
            }
            return Response::Display_file(last_path, a);
        }
    }
    return ErrorResponse::Error_NotFound(a);
}

std::string Response::Get_delete(std::string path, LocationConfig info_location,
                                    Request test_request, Config a)
{

    (void)info_location, (void)test_request, (void)a;

    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) != 0)
    {
        if (errno == ENOENT)
            return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        return "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
    }
    if (S_ISDIR(path_stat.st_mode))
        return "HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n";

    if (access(path.c_str(), W_OK) != 0)
    {
        if (errno == EACCES)
            return "HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n";
        else if (errno == ENOENT)
            return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
    }

    int status = remove(path.c_str());
    if (status != 0)
    {
        if (errno == ENOTEMPTY || errno == EACCES)
            return "HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n";
        else if (errno == ENOENT)
            return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
    }
    return "HTTP/1.1 204 No Content\r\nContent-Length: 0\r\n\r\n";
}
