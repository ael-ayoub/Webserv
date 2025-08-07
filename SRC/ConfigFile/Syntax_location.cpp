#include "../../INCLUDES/Syntax_location.hpp"

void syntax_location::check_locations(Vector_str str, int *i)
{
    size_t store_i = *i;
    int check = 0;
    int brace_cout = 0;

    if (str[*i].find('{') != std::string::npos)
        brace_cout++;
    while (store_i < str.size())
    {
        Vector_str tmp = ServerConfig::ft_splitv2(ServerConfig::remove_spaces(str[store_i]), ' ');
        // size_t j = 0;
        if ((tmp.size() != 3 || tmp[2] != "{") && tmp[0] == "location")
            throw Config::ErrorSyntax();
        
        if (tmp[0] == "methods")
            tmp[tmp.size() - 1].erase(tmp[tmp.size() - 1].size() - 1);
        if (tmp[0] == "upload_store"
            || (tmp[0] == "methods" && tmp[1] == "POST")
            || tmp[0] == "upload_enable")
        {
            check_upload(str, i);
            check = 1;
            break;
        }
        else if (tmp[0] == "cgi_pass")
        {
            check_cgi(str, i);
            check = 1;
            break;
        }
        if (str[store_i].find('}') != std::string::npos)
            brace_cout--;
        if (brace_cout == 0)
            break;
        store_i++;
    }
    if (check == 0)
        check_regular(str, i);
        // kmala
}

void syntax_location::check_regular(Vector_str str, int *i)
{
     (*i)++;
    while ((unsigned int)*i < str.size())
    {
        Vector_str tmp = ServerConfig::ft_splitv2(ServerConfig::remove_spaces(str[*i]), ' ');

        if (tmp[0][0] == '}')
        {
            if (tmp[0].size() != 1 || tmp.size() != 1)
                throw Config::ErrorSyntax();
            break;
        }

        if (tmp[tmp.size() - 1][tmp[tmp.size() - 1].size() - 1] == ';')
            tmp[tmp.size() - 1].erase(tmp[tmp.size() - 1].size() - 1);
        else
            throw Config::ErrorSyntax();

        if (tmp[0] == "methods")
        {
            if (tmp.size() < 2
                || (tmp[1] != "GET" && tmp[1] != "POST" && tmp[1] != "DELETE"))
                throw Config::ErrorSyntax();
        }
        else if (tmp[0] == "root")
        {
            if (tmp.size() != 2 || tmp[1].size() == 0 || tmp[1][0] != '/')
                throw Config::ErrorSyntax();
        }
        else if (tmp[0] == "index")
        {
            if (tmp.size() != 2 || tmp[1].size() == 0)
                throw Config::ErrorSyntax();
        }
        else if (tmp[0] == "autoindex")
        {
            if (tmp.size() != 2 || (tmp[1] != "off" && tmp[1] != "on"))
                throw Config::ErrorSyntax();
        }
        else
        {
            std::cout << tmp[0] << std::endl;
            throw Config::ErrorSyntax();
        }
        (*i)++;
    }
}

void syntax_location::check_cgi(Vector_str str, int *i)
{
    (*i)++;
    while ((unsigned int)*i < str.size())
    {
        Vector_str tmp = ServerConfig::ft_splitv2(ServerConfig::remove_spaces(str[*i]), ' ');

        if (tmp[0][0] == '}')
        {
            if (tmp[0].size() != 1 || tmp.size() != 1)
                throw Config::ErrorSyntax();
            break;
        }

        if (tmp[tmp.size() - 1][tmp[tmp.size() - 1].size() - 1] == ';')
            tmp[tmp.size() - 1].erase(tmp[tmp.size() - 1].size() - 1);
        else
            throw Config::ErrorSyntax();

        if (tmp[0] == "methods")
        {
            if (tmp.size() < 2
                || (tmp[1] != "GET" && tmp[1] != "POST" && tmp[1] != "DELETE"))
                throw Config::ErrorSyntax();
        }
        else if (tmp[0] == "cgi_pass")
        {
            if (tmp.size() != 2 || tmp[1].size() == 0 || tmp[1][0] != '/')
                throw Config::ErrorSyntax();
        }
        else
            throw Config::ErrorSyntax();
        (*i)++;
    }
}

void syntax_location::check_upload(Vector_str str, int *i)
{
    // std::cout << "found upload" << str[*i] << "\n";
    (*i)++;
    while ((unsigned int)*i < str.size())
    {
        Vector_str tmp = ServerConfig::ft_splitv2(ServerConfig::remove_spaces(str[*i]), ' ');

        if (tmp[0][0] == '}')
        {
            if (tmp[0].size() != 1 || tmp.size() != 1)
                throw Config::ErrorSyntax();
            break;
        }

        if (tmp[tmp.size() - 1][tmp[tmp.size() - 1].size() - 1] == ';')
            tmp[tmp.size() - 1].erase(tmp[tmp.size() - 1].size() - 1);
        else
            throw Config::ErrorSyntax();
        
        if (tmp[0] == "methods")
        {
            if (tmp.size() != 2 || tmp[1] != "POST")
                throw Config::ErrorSyntax();
        }
        else if (tmp[0] == "upload_store")
        {
            if (tmp.size() != 2 || tmp[1].size() == 0 || tmp[1][0] != '/')
                throw Config::ErrorSyntax();
        }
        else if (tmp[0] == "upload_enable")
        {
            if (tmp.size() != 2 || (tmp[1] != "on" && tmp[1] != "off"))
                throw Config::ErrorSyntax();
        }
        else
            throw Config::ErrorSyntax();
        (*i)++;
    }
}
