#pragma once

#include "Webserv.hpp"

class LocationConfig;
class ServerConfig
{
    private:
        Vector_str server_config;
        Vector_str server_data;
        std::vector<LocationConfig> Location_Config;
        std::pair<std::string, int> ip_port;
        Vector_str server_name;
        std::string path_server_root;
        std::string path_server_index;
        size_t client_max_body_size;
        // std::vector<std::pair<int, std::string> > errorStatus_pathError;
        std::vector<std::map<int, std::string> > errorStatus_pathError;

    public:
        LocationConfig get_Location_Config(std::string path);
        
        int    parse_config(Vector_str server_configg, int from, int to);
        void    print_conf();
        static std::pair<std::string, std::string> ft_splito(std::string str, char c);
        static Vector_str ft_splitv2(std::string str, char c);
        LocationConfig get_conf(std::string path);
        void    store_server_info();
        static std::string remove_spaces(std::string str);
        static Vector_str parse_line(std::string line);
        void print_info_server();
        int check_configFile();
};
