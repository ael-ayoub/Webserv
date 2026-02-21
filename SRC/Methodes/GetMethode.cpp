#include "../../INCLUDES/Webserv.hpp"

std::string clean_string(std::string str)
{
    std::string re;
    int i = 0;
    while (str[i])
    {
        if (str[i] == '/' && str[i + 1] != '/')
            re += '/';
        else if (str[i] != '/')
            re += str[i];
        i++;
    }
    
    return re;
}

std::string gcwd()
{
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
        return std::string(buffer);
    return "";
}

static std::string strip_query(const std::string &path)
{
    const size_t q = path.find('?');
    if (q == std::string::npos)
        return path;
    return path.substr(0, q);
}

std::string Methodes::GetMethod(Config& a, Request& test_request, ServerConfig& Servers_Config)
{
    (void)a;
    std::string s;
    std::string body, line, last_path;
    const std::string request_path = strip_query(test_request.get_path());
    LocationConfig info_location = Servers_Config.get_conf(request_path);
    
    std::string path = gcwd() + info_location.get_root() + request_path;
    struct stat statbuf;
    if (stat((path).c_str(), &statbuf) == 0)
    {
        if (S_ISDIR(statbuf.st_mode) && path[path.size() - 1] != '/')
        {
            std::string res;
            res += "HTTP/1.1 301 Moved Permanently\r\n";
            res += "Location: " + clean_string(request_path) + '/' + "\r\n";
            res += "\r\n";
            return res;
        }
    }
    info_location = Servers_Config.get_conf(clean_string(request_path));
    
    if (test_request.get_method() == "GET" && info_location.get_method("GET") == true)
    {
        return Response::Get_response(path, info_location, test_request, a);
    }
    else if (test_request.get_method() == "DELETE" && info_location.get_method("DELETE") == true)
    {
        return Response::Get_delete(path, info_location, test_request, a);
    }
    else
    {

        return ErrorResponse::Error_MethodeNotAllowed(a);
    }

	return "";
}
