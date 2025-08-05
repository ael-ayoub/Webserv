#include "../../INCLUDES/ServerConfig.hpp"

void    ServerConfig::parse_config(Vector_str server_configg, int from, int to)
{
    while (from < to)
    {
        server_config.push_back(server_configg[from]);
        from++;
    }
    this->store_server_info();
    this->print_info_server();
}

Vector_str ServerConfig::parse_line(std::string line)
{
    std::string tmp = remove_spaces(line);
    Vector_str re;

    // size_t pos = tmp.find(' ');
    size_t pos = tmp.find(' ');
    size_t to = 0;
    while (pos != std::string::npos)
    {
        re.push_back(tmp.substr(to, pos - to));
        to = pos + 1;
        pos = tmp.find(' ', to);
    }
    if (to != std::string::npos)
      re.push_back(tmp.substr(to));
    // size_t i = 0;
    // while (i < re.size())
    // {
    //   std::cout << "i have :" << re[i] << "." << std::endl;
    //   i++;
    // }
    // re.push_back(tmp.substr(0, pos));
    // while (pos != std::string::npos)
    // {
    //     re.push_back(tmp.substr(pos, to));
    //     pos = tmp.find(' ');
    // }
    return re;
}

void    ServerConfig::store_server_info()
{
    size_t i = 0;
    while (i < server_config.size())
    {
        std::pair<std::string, std::string> a = this->ft_splito(server_config[i], ' ');
        // std::cout << "first :" << a.first << "\nseconde : " << a.second << std::endl;
        if (a.first == "listen")
        {
            std::pair<std::string, std::string> tmp = this->ft_splito(a.second, ':');
            int port_tmp;
            std::istringstream(tmp.second) >> port_tmp;
            ip_port = std::make_pair(tmp.first, port_tmp);
        }
        else if (a.first == "server_name")
            server_name = a.second;
        else if (a.first == "client_max_body_size")
            std::istringstream(a.second) >> client_max_body_size;
        else if (a.first == "root")
            path_server_root = a.second;
        else if (a.first == "index")
            path_server_index = a.second;
        else if (a.first == "error_page")
        {
            Vector_str tmp = parse_line(a.second);
            size_t j = 0;
            while (j < tmp.size() - 1)
            {
                int status_tmp;
                std::istringstream(tmp[j]) >> status_tmp;
                std::map<int, std::string> error;
                error[status_tmp] = tmp[tmp.size() - 1];
                errorStatus_pathError.push_back(error);
                j++;
            }
        }
        else if (a.first == "location")
        {
            // std::cout << "been called " << std::endl;
            LocationConfig tmp;
            std::pair<std::string, std::string> path = this->ft_splito(a.second, ' ');
            tmp.parse_locationConfig(server_config, &i, path.first);
            Location_Config.push_back(tmp);
        }
        i++;
        // std::cout << "i = " << i << "server size is " << server_config.size() << std::endl;
    }
}

void ServerConfig::print_info_server()
{
    std::cout << "--------------------------------------\n";
    std::cout << "ip :" << ip_port.first << "." << std::endl;
    std::cout << "port_num :" << ip_port.second << "." << std::endl;
    std::cout << "server name :" << server_name << "." << std::endl;
    std::cout << "root :" << path_server_root << "." << std::endl;
    std::cout << "index :" << path_server_index << "." << std::endl;
    std::cout << "client_max_body_size :" << client_max_body_size << "." << std::endl;
    size_t i = 0;
    while (i < errorStatus_pathError.size())
    {
        std::map<int, std::string>::iterator ii = errorStatus_pathError[i].begin();
        while (ii != errorStatus_pathError[i].end())
        {
            std::cout << "error_status :"<< ii->first;
            std::cout << " path :" << ii->second << "." << std::endl;
            ii++;
        }
        i++;
    }
    i = 0;
    while (i < Location_Config.size())
    {
        Location_Config[i].print_info();
        i++;
    }
    
}

std::string ServerConfig::remove_spaces(std::string str)
{
    int found_char = 0;
    int i = 0;
    std::string tmp;

    while (str[i])
    {
        if (!std::isspace(str[i]))
        {
            if ((i != 0 && std::isspace(str[i - 1])) && found_char == 1)
            {
                tmp += ' ';
            }
            found_char = 1;
            tmp += str[i];
        }
        i++;
    }
    return tmp;
}
std::pair<std::string, std::string> ServerConfig::ft_splito(std::string str, char c)
{
    // int i = 0;
    std::string tmp;
    std::string first;
    std::string seconde;
    Vector_str re;

    tmp = remove_spaces(str);
    size_t pos = tmp.find(c);
    if (pos != std::string::npos)
    {
        first = tmp.substr(0, pos);
        seconde = tmp.substr(pos + 1);
        if (seconde[seconde.size() - 1] == ';')
            seconde.erase(seconde.size() - 1);
    }
    return std::make_pair(first, seconde);
}

Vector_str ServerConfig::get_conf()
{
    return server_config;
}

void    ServerConfig::print_conf()
{
    size_t i = 0;
    while (i < server_config.size())
    {
        std::cout << server_config[i] << std::endl;
        i++;
    }
}
