#pragma once

#include "Webserv.hpp"

class LocationConfig;
class Request;
class Response
{
    private:
        /* data */
    public:
        static std::string Display_dir(std::string path, LocationConfig info_location, Config a
    , std::string last_path);
        static std::string Display_file(std::string last_path, Config a, Request test_request);
};
