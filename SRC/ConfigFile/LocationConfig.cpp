#include "../../INCLUDES/LocationConfig.hpp"

LocationConfig::LocationConfig()
    : path_index("None"), autoindex(false), GET_methode(false),
        post_methode(false), delete_methode(false), upload_enable(false),
        redirection(false)
{
    path_location = "None";
    return_location.second = "";
}

std::string LocationConfig::GetLocationPath()
{
    return return_location.second;
}

bool LocationConfig::GetRedirectionBool()
{
    return redirection;
}

void LocationConfig::set_path_location()
{
    path_location = "/";
}

void LocationConfig::set_methode()
{
    GET_methode = true;
}

void LocationConfig::set_autoindex()
{
    autoindex = true;
}

void LocationConfig::set_path_root()
{
    path_root = "/home/ael-aiss/Desktop/Webserv/STATIC";
}

bool LocationConfig::get_autoIndex()
{
    return autoindex;
}

bool LocationConfig::get_uploadEnable()
{
    return upload_enable;
}

std::string LocationConfig::get_uploadStore()
{
    return upload_store;
}

std::string LocationConfig::get_pathIndex()
{
    return path_index;
}

std::string LocationConfig::get_root()
{
    return path_root;
}

std::string LocationConfig::get_path()
{
    return path_location;
}

bool LocationConfig::has_cgi_for_extension(const std::string &ext)
{
    return cgi_pass.find(ext) != cgi_pass.end();
}

std::string LocationConfig::get_cgi_binary(const std::string &ext)
{
    std::map<std::string, std::string>::iterator it = cgi_pass.find(ext);
    if (it == cgi_pass.end())
        return "";
    return it->second;
}

bool LocationConfig::get_method(std::string request_method)
{
    if (GET_methode == true && request_method == "GET")
        return true;
    if (post_methode == true && request_method == "POST")
        return true;
    if (delete_methode == true && request_method == "DELETE")
        return true;
    if (redirection == true)
        return true;
    return false;
}

void LocationConfig::parse_locationConfig(Vector_str str, size_t *start, std::string path)
{
    int brace_cout = 0;
    if (str[*start].find('{') != std::string::npos)
            brace_cout++;
    (*start)++;
    // std::cout << "------------\n";
    path_location = path;
    while (*start < str.size())
    {
        if (str[*start].find('{') != std::string::npos)
            brace_cout++;
        if (str[*start].find('}') != std::string::npos)
        {
            brace_cout--;
            if (brace_cout == 0)
                break;
        }
        std::pair<std::string, std::string> a = ServerConfig::ft_splito(str[*start], ' ');
        if (a.first == "root")
            path_root = a.second;
        else if (a.first == "index")
            path_index = a.second;
        else if (a.first == "autoindex")
        {
            if (a.second == "on")
                autoindex = true;
        }
        else if (a.first == "methods")
        {
            Vector_str tmp_methods = ServerConfig::parse_line(a.second);
            size_t tmp_i = 0;
            while (tmp_i < tmp_methods.size())
            {
                if (tmp_methods[tmp_i] == "GET")
                    GET_methode = true;
                if (tmp_methods[tmp_i] == "POST")
                    post_methode = true;
                if (tmp_methods[tmp_i] == "DELETE")
                    delete_methode = true;
                tmp_i++;
            }
        }
        else if (a.first == "upload_enable")
        {
            if (a.second == "on")
                upload_enable = true;
            else
                upload_enable = false;
        }
        else if (a.first == "upload_store")
            upload_store = a.second;
        else if (a.first == "cgi_pass")
        {
            Vector_str parts = ServerConfig::parse_line(a.second);
            if (parts.size() == 2)
                cgi_pass[parts[0]] = parts[1];
        }
        else if (a.first == "return")
        {
            int status_code;
            redirection = true;
            std::pair<std::string, std::string> tmp = ServerConfig::ft_splito(a.second, ' ');
            std::istringstream(tmp.first) >> status_code;
            return_location = std::make_pair(status_code, tmp.second);
        }
        // std::cout << str[i] << " start is " << start << "\n";
        (*start)++;
    }
}

void LocationConfig::print_info()
{
    std::cout << "--------location : " << path_location << "-----------\n\n" << std::endl;
    std::cout << "index : " << path_index << std::endl;
    std::cout << "root : " << path_root << std::endl;
    std::cout << "autoindex : " <<autoindex << std::endl;
    std::cout << "GET : " << GET_methode << std::endl;
    std::cout << "POST : " << post_methode << std::endl;
    std::cout << "Delete : " << delete_methode << std::endl;
    std::cout << "upload_enable : " << upload_enable << std::endl;
    std::cout << "upload_store : " << upload_store << std::endl;
    std::cout << "status return : " << return_location.first << std::endl;
    std::cout << "return_path : " << return_location.second << std::endl;
}