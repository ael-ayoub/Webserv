#pragma once

#include "Webserv.hpp"

class Request
{
    private:
        std::string method;
        std::string path;
    public:
        void    parse_request(char *buffer);
        std::string get_path();
        std::string get_method();
};
