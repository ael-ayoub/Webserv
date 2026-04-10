#pragma once

#include "Webserv.hpp"

class Request
{
    private:
        std::string method;
        std::string path;
        std::string HTTP;
        int port;
        std::string hostname;
        std::string content_length;
    
    
    public:
        Request() : content_length("NONE") {}
        std::string check_headerline(std::string str, Config &a);
        std::string check_requestline(std::string str, Config a);
        std::string check_request(std::string str, Config a);
        std::string parse_request(std::string buffer, Config a);
        std::string get_path();
        std::string get_method();
        size_t get_content_length();
    
    
        int get_port();
        std::string get_Hostname();
};


