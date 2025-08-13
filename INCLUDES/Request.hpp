#pragma once

#include "Webserv.hpp"

class Request
{
    private:
        std::string method;
        std::string path;
    public:
        bool check_headerline(std::string str);
        bool check_requestline(std::string str);
        bool check_request(std::string str);
        void    parse_request(char *buffer);
        std::string get_path();
        std::string get_method();
};
