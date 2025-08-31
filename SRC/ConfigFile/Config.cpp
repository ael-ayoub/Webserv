#include "../../INCLUDES/Config.hpp"

ServerConfig Config::get_server_config()
{
    return Servers_Config[0];
}

std::vector<ServerConfig> Config::get_allserver_config()
{
    return Servers_Config;
}

std::string Config::get_mine(std::string path)
{
    size_t dot = path.rfind('.');
    if (dot == std::string::npos || dot == path.size() - 1)
        return "";
    std::string tmp = path.substr(dot + 1);
    if (tmp == "html")
        return "text/html";
    else if (tmp == "css")
        return "text/css";
    else if (tmp == "js")
        return "application/javascript";
    else if (tmp == "gif")
        return "image/gif";
    else if (tmp == "txt")
        return "text/plain";
    else if (tmp == "json")
        return "application/json";
    return "text/plain";
}

int Config::store_file(std::string path_of_Cfile)
{
    std::ifstream file(path_of_Cfile.c_str());
    if (!file.is_open())
    {
        std::cerr << "Error" << std::endl;
        return 1;
    }
    std::string line;
    while (std::getline(file, line))
    {
        file_lines.push_back(line);
    }
    file.close();
    this->stores_config();
    return 0;
}

LocationConfig Config::get_info_location(std::string path)
{
    LocationConfig tmp;
    size_t i = 0;
    while (i < Servers_Config.size())
    {
        tmp = Servers_Config[i].get_conf(path);
        if (tmp.get_path() != "None")
            return tmp;
        i++;
    }
    return tmp;
}

void Config::print_confiFile()
{
    // size_t i = 0;
    // std::cout << "....\n";
    // while (i < Servers_Config.size())
    // {
    //     Servers_Config[i].print_conf();
    //     i++;
    //     std::cout << "....\n";
    // }
}

int    Config::stores_config()
{
    while (first_last.first != -1 && first_last.second != -1)
    {
        ServerConfig tmp;

        first_last = this->get_firstlast();
        if (first_last.first == -2)
            throw Config::ErrorSyntax();
        if (first_last.first == -1 || first_last.second == -1)
            break;
        tmp.parse_config(file_lines, first_last.first, first_last.second);
        Servers_Config.push_back(tmp);
        first_last.first = first_last.second + 1;
    }
    //check if a fallback is set
    return 0;
}

std::string  Config::remove_whitespaces(std::string line)
{
    std::string tmp;
    int i = 0;

    while (line[i])
    {
        if (!std::isspace(line[i]))
            tmp += line[i];
        i++;
    }
    return tmp;
}

std::pair<int, int> Config::get_firstlast()
{
    size_t  i = first_last.first;
    int     found_it = 0;
    int     brace_cout = 0;
    int     first = -1;
    int     last = -1;

    while (i < file_lines.size() && found_it == 0)
    {
        std::string line = Config::remove_whitespaces(file_lines[i]);
        if (i == 0 && line != "server{")
        {
            first = -2;
            break;
        }

        if (line.find('{') != std::string::npos)
        {
            brace_cout++;
        }
        else if (line.find('}') != std::string::npos)
        {
            if (line.size() != 1)
                throw Config::ErrorSyntax();
            brace_cout--;
        }

        if (line == "server{")
            first = i;
        else if (line == "}" && brace_cout == 0)
        {
            last = i + 1;
            found_it = 1;
        }

        i++;
    }
    if (brace_cout != 0)
        first = -2;
    if (first == -1 && last != -1)
        first = -2;
    return std::make_pair(first, last);
}

Vector_str Config::get_file_lines()
{
    return file_lines;
}

Config::Config()
{
    first_last = std::make_pair(0, 0);
    start = 0;
}
