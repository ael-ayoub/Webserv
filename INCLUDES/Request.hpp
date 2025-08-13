#pragma once

#include "Webserv.hpp"

class Request
{
    private:
        std::string method;
        std::string path;
    public:
        std::string default_response_error(Config a);
        std::string response_error(Config a, std::string path);
        void    parse_request(char *buffer);
        std::string get_path();
        std::string get_method();
};
