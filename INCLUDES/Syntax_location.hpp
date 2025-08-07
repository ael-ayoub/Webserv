#pragma once

#include "Webserv.hpp"

class syntax_location
{
    private:
    public:
        // syntax_location();
        void check_locations(Vector_str str, int *i);
        void check_upload(Vector_str str, int *i);
        void check_cgi(Vector_str str, int *i);
        void check_regular(Vector_str str, int *i);
};