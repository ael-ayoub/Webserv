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
        std::string server_name;
        std::string path_server_root;
        std::string path_server_index;
        size_t client_max_body_size;
        // std::vector<std::pair<int, std::string> > errorStatus_pathError;
        std::vector<std::map<int, std::string> > errorStatus_pathError;

    public:
        void    parse_config(Vector_str server_configg, int from, int to);
        void    print_conf();
        static std::pair<std::string, std::string> ft_splito(std::string str, char c);
        Vector_str get_conf();
        void    store_server_info();
        static std::string remove_spaces(std::string str);
        static Vector_str parse_line(std::string line);
        void print_info_server();
};
