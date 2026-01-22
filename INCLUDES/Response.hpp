#pragma once

#include "Webserv.hpp"

class LocationConfig;
class Request;
class Response
{
    private:
        /* data */
    public:
        static std::string Display_dir(std::string path, LocationConfig info_location);
        static std::string Display_file(std::string last_path, Config a);
        static std::string Get_response(std::string path, LocationConfig &info_location,
                                        Request test_request, Config a);
        static std::string Get_delete(std::string path, LocationConfig info_location,
                                        Request test_request, Config a);
};
