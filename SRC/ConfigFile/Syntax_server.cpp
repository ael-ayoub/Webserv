#include "../../INCLUDES/Syntax_server.hpp"

syntax_server::syntax_server()
    : listen_check(false), client_max_body_size_check(false), error_page_check(false)
{
}

void syntax_server::check_server_syntax(Vector_str server)
{
    size_t i = 0;
    if (server[i][0] != 's')
        throw Config::ErrorSyntax();
    std::string tmp = Config::remove_whitespaces(server[i]);
    i++;
    while (i < server.size())
    {

        Vector_str tmp = ServerConfig::parse_line(server[i]);
        if (tmp[0] == "location")
        {
            size_t k = 0;
            while (k < location_path.size())
            {
                if (tmp[1] == location_path[k])
                    throw Config::ErrorSyntax();
                k++;
            }
            location_path.push_back(tmp[1]);
            if (listen_check == false || client_max_body_size_check == false
                || error_page_check == false)
                throw Config::ErrorSyntax();
            // break;

            syntax_location loc;
            loc.check_locations(server, (int *)&i);
        }
        if (tmp[0][0] && server[i].find('{') == std::string::npos
            && server[i].find('}') == std::string::npos
            && tmp[tmp.size() - 1][tmp[tmp.size() - 1].size() - 1] != ';')
            throw Config::ErrorSyntax();
        // if (tmp[0] != "error_page" && tmp[0][0] && tmp.size() != 2)
        //     throw Config::ErrorSyntax();
        // start checking server info
        // std::cout << "he is \n";

        if (tmp[0] == "listen")
            check_listen(tmp);
        else if (tmp[0] == "client_max_body_size")
            check_body_size(tmp);
        else if (tmp[0] == "error_page")
            check_error_page(tmp);
        else if (tmp[0].size() != 0 && tmp[0] != "location" 
                && tmp[0] != "}" && tmp[0] != "server_name"
                && tmp[0] != "client_max_body_size")
        {
            throw Config::ErrorSyntax();
        }
        i++;
    }

}

void syntax_server::check_number(std::string str)
{
    int j = 0;
    while (str[j])
    {
        // std::cout << "im in " << str[j] << "\n";
        if (!std::isdigit(str[j]))
            throw Config::ErrorSyntax();
        j++;
    }
}

void syntax_server::check_error_page(Vector_str str)
{
    if (str.size() < 3)
        throw Config::ErrorSyntax();
    error_page_check = true;
    size_t i = 1;
    while (i < str.size() - 1)
    {
        check_number(str[i]);
        int num;
        std::istringstream(str[i]) >> num;
        if (num < 400 || num > 599)
            throw Config::ErrorSyntax();
        i++;
    }
    if (str[i][0] != '/')
        throw Config::ErrorSyntax();
}

void syntax_server::check_body_size(Vector_str str)
{
    if (client_max_body_size_check == true || str.size() > 2)
        throw Config::ErrorSyntax();
    client_max_body_size_check = true;
    str[1] = str[1].erase(str[1].size() - 1); // remove ;
    check_number(str[1]);
    size_t size;
    std::istringstream(str[1]) >> size;
    if (size > INT_MAX)
        throw Config::ErrorSyntax();
}

void syntax_server::check_listen(Vector_str str)
{
    int num;
    if (listen_check == true || str.size() > 2)
        throw Config::ErrorSyntax();
    listen_check = true;
    str[1] = str[1].erase(str[1].size() - 1); // remove ;
    if (str[1].find(':') != std::string::npos)
    {
        Vector_str info = ServerConfig::ft_splitv2(str[1], ':');
        // std::cout << "size of them " << info[0].size() << std::endl;
        if (info.size() > 2 || info[0].size() == 0 || info[1].size() == 0)
            throw Config::ErrorSyntax();
        check_number(info[1]);
        std::istringstream(info[1]) >> num;
        if (num < 1 || num > 65535)
            throw Config::ErrorSyntax();
        Vector_str ip_port = ServerConfig::ft_splitv2(info[0], '.');
        if (ip_port.size() != 4)
            throw Config::ErrorSyntax();
        // std::cout << "size is " << ip_port.size() << "\n";
        size_t j = 0;
        while (j < ip_port.size())
        {
            check_number(ip_port[j]);
            std::istringstream(ip_port[j]) >> num;
            if (num < 0 || num > 255)
                throw Config::ErrorSyntax();
            j++;
        }
    }
    else
        throw Config::ErrorSyntax();
}