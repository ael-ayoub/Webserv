#include "../../INCLUDES/Webserv.hpp"


std::string Methodes::GetMethod(Config a, char *buffer)
{
    (void)a;
    std::string s;
    std::string body, line, last_path;
    Request test_request;
    test_request.parse_request(buffer);
    // std::cout << "Get request of " << test_request.get_path() << std::endl;
    if (test_request.get_path() == "/favicon.ico")
        return std::string(); ///////////////////// check heree the favicon.co why he display everytume
    LocationConfig info_location = a.get_info_location(test_request.get_path());

    std::string path = info_location.get_root() + test_request.get_path();
    struct stat statbuf;

    if (stat(path.c_str(), &statbuf) == 0)
    {
        if (S_ISDIR(statbuf.st_mode))
        {
            if (info_location.get_pathIndex() != "None")
            {
                last_path = info_location.get_root() + "/" + info_location.get_pathIndex();
                return Response::Display_file(last_path, a, test_request);
            }
            else
            {
                if (info_location.get_autoIndex() == false && info_location.get_pathIndex() == "None")
                    return test_request.response_error(a, last_path);
                return Response::Display_dir(path, info_location, a, last_path);
            }
        }
        else
        {
            last_path = info_location.get_root() + test_request.get_path();
            return Response::Display_file(last_path, a, test_request);
        }
    }
    else
    {
        return test_request.response_error(a, last_path);
    }
	return "";
}
