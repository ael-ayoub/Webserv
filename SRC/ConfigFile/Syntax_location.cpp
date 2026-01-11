#include "../../INCLUDES/Syntax_location.hpp"

syntax_location::syntax_location()
    : methods(false), upload_enable(false), upload_store(false),
     cgi(false), root(false), auto_index(false)
{
}
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
        {
            // std::cout << "he is \n";

            throw Config::ErrorSyntax();
        }
        
        if (tmp[0] == "methods")
            tmp[tmp.size() - 1].erase(tmp[tmp.size() - 1].size() - 1);
        if (tmp[0] == "upload_store"
            || tmp[0] == "upload_enable")
        {
            // std::cout << "he is \n";
            check_upload(str, i);
            check = 1;
            break;
        }
        else if (tmp[0] == "cgi_pass")
        {
            // std::cout << "he is \n";
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
    {
        check_regular(str, i);
    }
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
            if (methods == true)
                throw Config::ErrorSyntax();
            methods = true;
            if (tmp.size() < 2)
                throw Config::ErrorSyntax();
            size_t k = 1;
            while (k < tmp.size())
            {
                if (tmp[k] != "POST" && tmp[k] != "DELETE"
                    && tmp[k] != "GET")
                    throw Config::ErrorSyntax();
                k++;
            }
        }
        else if (tmp[0] == "root")
        {
            if (root == true)
                throw Config::ErrorSyntax();
            root = true;
            if (tmp.size() != 2 || tmp[1].size() == 0 || tmp[1][0] != '/')
                throw Config::ErrorSyntax();
        }
        else if (tmp[0] == "index")
        {
            if (index == true)
                throw Config::ErrorSyntax();
            index = true;
            if (tmp.size() != 2 || tmp[1].size() == 0)
                throw Config::ErrorSyntax();
        }
        else if (tmp[0] == "autoindex")
        {
            if (auto_index == true)
                throw Config::ErrorSyntax();
            auto_index = true;
            if (tmp.size() != 2 || (tmp[1] != "off" && tmp[1] != "on"))
                throw Config::ErrorSyntax();
        }
        else
        {
            throw Config::ErrorSyntax();
        }
        (*i)++;
    }
    if (auto_index == false || root == false
        || methods == false)
        throw Config::ErrorSyntax();
    auto_index = false;
    index = false;
    methods = false;
    root = false;
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

        // if (tmp[0] == "methods")
        // {
        //     methods = true;
        //     if (tmp.size() < 2
        //         || (tmp[1] != "GET" && tmp[1] != "POST" && tmp[1] != "DELETE"))
        //     {

        //         throw Config::ErrorSyntax();
        //     }
        // }
        // else 
        if (tmp[0] == "root")
        {
            if (root == true)
                throw Config::ErrorSyntax();
            root = true;
            if (tmp.size() != 2 || tmp[1].size() == 0 || tmp[1][0] != '/')
                throw Config::ErrorSyntax();
        }
        else if (tmp[0] == "cgi_pass")
        {
            if (cgi == true)
                throw Config::ErrorSyntax();

            cgi = true;
            if (tmp.size() != 2 || tmp[1].size() == 0 || tmp[1][0] != '/')
            {

                throw Config::ErrorSyntax();
            }
        }
        else
        {
            throw Config::ErrorSyntax();
        }
        (*i)++;
    }
    if (cgi == false || root == false)
        throw Config::ErrorSyntax();
    cgi = false;
    root = false;
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
            if (methods == true)
                throw Config::ErrorSyntax();
            methods = true;
            if (tmp.size() != 2 || tmp[1] != "POST")
            {

                throw Config::ErrorSyntax();
            }
                // throw Config::ErrorSyntax();
        }
        else if (tmp[0] == "upload_store")
        {
            if (upload_store == true)
                throw Config::ErrorSyntax();
            upload_store = true;
            if (tmp.size() != 2 || tmp[1].size() == 0 || tmp[1][0] != '/')
                throw Config::ErrorSyntax();
        }
        else if (tmp[0] == "upload_enable")
        {
            if (upload_enable == true)
                throw Config::ErrorSyntax();
            upload_enable = true;
            if (tmp.size() != 2 || (tmp[1] != "on" && tmp[1] != "off"))
                throw Config::ErrorSyntax();
        }
        else
        {
            throw Config::ErrorSyntax();
        }
        (*i)++;
    }
    if (methods == false || upload_enable == false || upload_store == false)
        throw Config::ErrorSyntax();
    methods = false;
    upload_enable = false;
    upload_store = false;
}
