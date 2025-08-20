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
        bool GET_methode;
        bool post_methode;
        bool delete_methode;
        bool upload_enable;
        std::pair<int, std::string> return_location;

    public:
        LocationConfig();

        void set_path_location();
        void set_methode();
        void set_autoindex();
        void set_path_root();
        std::string get_path() const;
        std::string get_root() const ;
        std::string get_pathIndex();
        bool get_autoIndex();
        bool get_method(std::string request_method);
    void print_info();
    void parse_locationConfig(Vector_str str, size_t *start, std::string path);
    bool is_upload_enabled() const { return upload_enable; }
    std::string get_upload_store() const { return upload_store; }
};
