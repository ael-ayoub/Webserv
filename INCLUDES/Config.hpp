#pragma once

#include "Webserv.hpp"

class ServerConfig;
class LocationConfig;

class Config
{
    private:
        int start;
        std::pair<int, int> first_last;
        std::vector<ServerConfig> Servers_Config;
        Vector_str file_lines;

    public:
        Config();
        int                 store_file(std::string path_of_Cfile);
        void                get_port();
        int                stores_config();
        void                stores_server(std::pair<int, int> first_lastt);
        Vector_str          get_file_lines();
        static std::string         remove_whitespaces(std::string line);
        std::pair<int, int> get_firstlast();
        void print_confiFile();

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
