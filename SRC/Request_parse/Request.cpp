#include "../../INCLUDES/Request.hpp"

void Request::parse_request(char *buffer)
{
    int i = 0;
    std::string str(buffer);
    std::string tmp_str;
    while (str[i])
    {
        tmp_str += str[i];
        if (str[i] == '\n')
            break;
        i++;
    }
    Vector_str tmp = ServerConfig::ft_splitv2(tmp_str, ' ');
    size_t j = 0;
    while (j < tmp.size())
    {
        if (j == 0)
            method = tmp[0];
        else if (j == 1)
            path = tmp[1];
        j++;
    }
}

std::string Request::get_method()
{
    return method;
}

std::string Request::get_path()
{
    return path;
}
