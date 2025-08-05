#include "../../INCLUDES/Config.hpp"

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
    if (this->stores_config() == -1)
        return -1;
    return 0;
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
        // std::cout << "first :" << first_last.first << "seconde :" << first_last.second << "\n";
        if (first_last.first == -2)
            return -1;
        if (first_last.first == -1 || first_last.second == -1)
            break;
        if (tmp.parse_config(file_lines, first_last.first, first_last.second) == -1)
            return -1;
        Servers_Config.push_back(tmp);
        first_last.first = first_last.second + 1;
    }

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