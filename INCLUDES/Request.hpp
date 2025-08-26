#pragma once

#include "Webserv.hpp"
#include "Users.hpp"

class Request
{
private:
    std::string method;
    std::string path;
    std::string HTTP;
    int port;
    std::string hostname;

    User user;
    //std::map<std::string, std::string> session;
    std::pair<std::string, std::string> session;

public:
    std::pair<std::string, std::string> get_session();
    void set_session(const std::string& str);

    std::string check_headerline(std::string str, Config a);
    std::string check_requestline(std::string str, Config a);
    std::string check_request(std::string str, Config a);
    std::string parse_request(char *buffer, Config a);
    std::string get_path();
    std::string get_method();

    void get_user_form_request(const std::string &str);
    bool check_if_user_exist();
    void save_user_in_data();

    User get_user();
    int get_port();
    std::string get_Hostname();
};


