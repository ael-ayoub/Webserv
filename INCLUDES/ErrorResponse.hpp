#pragma once

#include "Webserv.hpp"

class ErrorResponse
{
    public:
        std::string path;
        static std::string check_errorstatus(std::vector<std::map<int, std::string> > error,
                                                int status_code, std::string& path);

        static std::string Error_PayloadTooLarge(Config &a);
        static std::string Error_MethodeNotAllowed(Config a);
        static std::string Error_NotFound(Config a);
        static std::string Error_BadRequest(Config &a);
        static std::string Error_Forbidden(Config a);
        static std::string Responde(Config a, std::string path, std::string &head, std::string status);

        static std::string default_response_error(std::string status_code);
        // static std::string response_error(Config a, std::string last);
};
