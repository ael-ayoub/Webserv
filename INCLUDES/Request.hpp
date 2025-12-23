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


public:

    std::string check_headerline(std::string str, Config a);
    std::string check_requestline(std::string str, Config a);
    std::string check_request(std::string str, Config a);
    std::string parse_request(char *buffer, Config a);
    std::string get_path();
    std::string get_method();


    int get_port();
    std::string get_Hostname();
};


