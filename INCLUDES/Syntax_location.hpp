#pragma once

#include "Webserv.hpp"

class syntax_location
{
    private:
        bool methods;
        bool upload_enable;
        bool upload_store;
        bool cgi;
        bool root;
        bool index;
        bool auto_index;
        // bool redirection;
    public:
        syntax_location();
        void check_locations(Vector_str str, int *i);
        void check_upload(Vector_str str, int *i);
        void check_cgi(Vector_str str, int *i);
        void check_regular(Vector_str str, int *i);
};