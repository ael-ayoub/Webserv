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
    // std::cout << "after test_req:   "<< re << std::endl;
    return re;
}

std::string gcwd()
{
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
        return std::string(buffer);
    return "";
}

std::string Methodes::GetMethod(Config a, Request test_request, ServerConfig Servers_Config)
{
    (void)a;
    std::string s;
    std::string body, line, last_path;
    // Request test_request;
    // std::string respon = test_request.parse_request(buffer, a);
    // if (respon != "NONE")
    //     return respon;
    // if (test_request.get_path() == "/favicon.ico")
    //     return std::string(); /// check heree the favicon.co why he display everytime
    
    LocationConfig info_location = Servers_Config.get_conf(test_request.get_path());
    
    // std::cout << "choose: " << info_location.get_path() << std::endl;
    // std::cout << "choose test_req: " << test_request.get_path() << std::endl;
    std::string path = gcwd() + info_location.get_root() + test_request.get_path();
    // std::cout << "path: " << path<< std::endl;
    struct stat statbuf;
    if (stat((path).c_str(), &statbuf) == 0)
    {
        if (S_ISDIR(statbuf.st_mode) && path[path.size() - 1] != '/')
        {
            std::string res;
            res += "HTTP/1.1 301 Moved Permanently\r\n";
            res += "Location: " + clean_string(test_request.get_path()) + '/' + "\r\n";
            res += "\r\n";
            return res;
        }
    }
    info_location = Servers_Config.get_conf(clean_string(test_request.get_path()));
    // std::cout << "we have now configure : " << clean_string(test_request.get_path()) << std::endl;
    // std::cout << "location working with is : " << info_location.get_path() << std::endl;
    // std::cout << "path is : " << info_location.GetLocationPath() << "\n";
    // std::cout << "bool is : " << info_location.GetRedirectionBool() << "\n";
    
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
