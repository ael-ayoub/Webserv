#pragma once

#include "Webserv.hpp"
#include "Users.hpp"

class ServerConfig;
class LocationConfig;

class Config
{
    private:
        int start;
        std::pair<int, int> first_last;
        std::vector<ServerConfig> Servers_Config;
        Vector_str file_lines;
        std::vector<std::pair<std::string , std::string> > sessions;
    public:
        Config();

        void set_sessions(std::pair<std::string , std::string> ss);
        std::vector<std::pair<std::string , std::string> > get_sessions();
        bool check_session(std::pair<std::string , std::string> ss);
        std::vector<ServerConfig> get_allserver_config();
        ServerConfig get_server_config();

        int                 store_file(std::string path_of_Cfile);
        void                get_port();
        int                stores_config();
        void                stores_server(std::pair<int, int> first_lastt);
        Vector_str          get_file_lines();
        static std::string         remove_whitespaces(std::string line);
        std::pair<int, int> get_firstlast();
        //void Get_all_users();

        void print_confiFile();

        std::string get_mine(std::string path);
        LocationConfig get_info_location(std::string path);

        class ErrorSyntax : public std::exception
        {
            public:
                const char* what() const throw()
                {
                    return "Error Syntax Config file";
                }
        };
};
