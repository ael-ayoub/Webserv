#include "../../INCLUDES/LocationConfig.hpp"

LocationConfig::LocationConfig()
    : autoindex(false), get_methode(false), post_methode(false), delete_methode(false)
{
}

void LocationConfig::parse_locationConfig(Vector_str str, size_t *start, std::string path)
{
    int brace_cout = 0;
    if (str[*start].find('{'))
            brace_cout++;
    (*start)++;
    std::cout << "------------\n";
    path_location = path;
    while (*start < str.size())
    {
        if (str[*start].find('{'))
            brace_cout++;
        if (str[*start].find('}'))
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
            autoindex = false;
        else if (a.first == "methods")
        {
            Vector_str tmp_methods = ServerConfig::parse_line(a.second);
            size_t tmp_i = 0;
            while (tmp_i < tmp_methods.size())
            {
                if (tmp_methods[tmp_i] == "GET")
                    get_methode = true;
                if (tmp_methods[tmp_i] == "POST")
                    post_methode = true;
                if (tmp_methods[tmp_i] == "DELETE")
                    delete_methode = true;
                tmp_i++;
            }
        }
        // std::cout << str[i] << " start is " << start << "\n";
        (*start)++;
    }
}

void LocationConfig::print_info()
{
    std::cout << "location : " << path_location << std::endl;
    std::cout << "index : " << path_index << std::endl;
    std::cout << "root : " << path_root << std::endl;
    std::cout << "autoindex : " <<autoindex << std::endl;
    std::cout << "GET : " << get_methode << std::endl;
    std::cout << "POST : " << post_methode << std::endl;
    std::cout << "Delete : " << delete_methode << std::endl;
}