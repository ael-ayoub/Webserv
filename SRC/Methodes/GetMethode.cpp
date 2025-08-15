#include "../../INCLUDES/Webserv.hpp"

std::string Methodes::GetMethod(Config a, Request test_request, ServerConfig Servers_Config)
{
    (void)a;
    std::string s;
    std::string body, line, last_path;
    // Request test_request;
    // std::string respon = test_request.parse_request(buffer, a);
    // if (respon != "NONE")
    //     return respon;
    if (test_request.get_path() == "/favicon.ico")
        return std::string(); ///////////////////// check heree the favicon.co why he display everytume

    LocationConfig info_location = Servers_Config.get_conf(test_request.get_path());

    std::string path = info_location.get_root() + test_request.get_path();

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
