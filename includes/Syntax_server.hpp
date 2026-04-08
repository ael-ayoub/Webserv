#pragma once

#include "Webserv.hpp"

class syntax_server
{
    private:
        bool listen_check;
        // bool serverName_check;
        bool client_max_body_size_check;
        bool error_page_check;
        Vector_str location_path;
    public:
        syntax_server();
        void check_server_syntax(Vector_str server);

        void check_listen(Vector_str str);
        void check_body_size(Vector_str str);
        static void check_number(std::string str);
        void check_error_page(Vector_str str);
};