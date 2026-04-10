#pragma once

#include "Webserv.hpp"

class LocationConfig
{
    private:
        std::string path_location;
        std::string path_root;
        std::string path_index;
        std::string upload_store;
        std::map<std::string, std::string> cgi_pass;
        bool autoindex;
        bool GET_methode;
        bool post_methode;
        bool delete_methode;
        bool upload_enable;
        std::pair<int, std::string> return_location;
        bool redirection;

    public:
        LocationConfig();

        std::string GetLocationPath();
        bool GetRedirectionBool();
        void set_path_location();
        void set_methode();
        void set_autoindex();
        void set_path_root();
        std::string get_path();
        std::string get_root();
        std::string get_pathIndex();
        bool get_autoIndex();
        bool get_uploadEnable();
        std::string get_uploadStore();

        bool has_cgi_for_extension(const std::string &ext);
        std::string get_cgi_binary(const std::string &ext);
        bool get_method(std::string request_method);
        void print_info();
        void parse_locationConfig(Vector_str str, size_t *start, std::string path);
};
