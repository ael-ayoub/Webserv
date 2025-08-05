#pragma once

#include "Webserv.hpp"

class ServerConfig;
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
        void                stores_config();
        void                stores_server(std::pair<int, int> first_lastt);
        Vector_str          get_file_lines();
        std::string         remove_whitespaces(std::string line);
        std::pair<int, int> get_firstlast();
        void print_confiFile();
};
