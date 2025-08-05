#pragma once

#include "Webserv.hpp"

class LocationConfig
{
    private:
        std::string path_location;
        std::string path_root;
        std::string path_index;
        std::string upload_store;
        bool autoindex;
        bool get_methode;
        bool post_methode;
        bool delete_methode;
        bool upload_enable;
        std::pair<int, std::string> return_location;

    public:
        LocationConfig();
        void print_info();
        void parse_locationConfig(Vector_str str, size_t *start, std::string path);
};
