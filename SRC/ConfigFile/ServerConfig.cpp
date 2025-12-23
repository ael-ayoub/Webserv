#include "../../INCLUDES/ServerConfig.hpp"

std::vector<std::map<int, std::string> > ServerConfig::get_error_status()
{
    return errorStatus_pathError;
}

std::pair<std::string, int> ServerConfig::get_ip()
{
    return ip_port;
}

int right_path(std::string Config_path, std::string path)
{
    int i = 0;
    while (path[i] && Config_path[i] && path[i] == Config_path[i])
    {
        i++;
    }
    if (Config_path[i] && path[i])
        return -1;
    if (i != 0 && (Config_path[i - 1] != '/' || path[i - 1] != '/'))
        return -1;
    return i;
}
std::string get_current_pathh()
{
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
        return std::string(buffer);
    return "";
}

LocationConfig ServerConfig::get_Location_Config(std::string path)
{
    LocationConfig tmp;
    int store_id = -1, j_old = -1;

    size_t i = 0;
    while (i < Location_Config.size())
    {
        if (Location_Config[i].get_path() == path)
            return Location_Config[i];
        int j = right_path(Location_Config[i].get_path(), path);
        
        if (j > j_old)
            store_id = i;

        if (j != j_old)
            j_old = j;
        i++;
    }
    // std::cout << "first location is : " << Location_Config[0].get_path() << std::endl;
    // std::cout << "seconde location is : " << Location_Config[1].get_path() << std::endl;
    // if (store_id != 0 && path != "/")
    // {
        // std::cout << "returning this and the index is " << store_id << std::endl;
        // std::cout << "index is " << store_id << std::endl;
        return Location_Config[store_id];
    // }
    // return tmp;
}

int    ServerConfig::check_configFile()
{
    syntax_server tmp;
    tmp.check_server_syntax(server_config);
    return 0;
}

int    ServerConfig::parse_config(Vector_str server_configg, int from, int to)
{
    while (from < to)
    {

        server_config.push_back(server_configg[from]);
        from++;
    }
    this->check_configFile();
    this->store_server_info();
    // std::cout << "------------------------------------\n";
    // this->print_info_server();
    // std::cout << "------------------------------------\n";
    return 0;
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

bool ServerConfig::checkAdd_fallback()
{
    size_t i = 0;
    while (i < Location_Config.size())
    {
        if (Location_Config[i].get_path() == "/")
            return true;
        i++;
    }
    LocationConfig tmp;
    tmp.set_autoindex();
    tmp.set_methode();
    tmp.set_path_root();
    tmp.set_path_location();
    Location_Config.push_back(tmp);
    return false;
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
            if (a.second.find(':') != std::string::npos)
            {
                std::pair<std::string, std::string> tmp = this->ft_splito(a.second, ':');
                int port_tmp;
                std::istringstream(tmp.second) >> port_tmp;
                if (tmp.first == "localhost")
                    ip_port = std::make_pair("127.0.0.1", port_tmp);
                else
                    ip_port = std::make_pair(tmp.first, port_tmp);
            }
            else
            {
                int port_tmp;
                std::istringstream(a.second) >> port_tmp;
                ip_port = std::make_pair("", port_tmp);
            }
        }
        else if (a.first == "server_name")
            server_name = ft_splitv2(a.second, ' ');
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
    if (checkAdd_fallback() == false)
        std::cout << "the fallback not found\n";
}

void ServerConfig::print_info_server()
{
    size_t i = 0;
    std::cout << "--------------------------------------\n";
    std::cout << "ip :" << ip_port.first << "." << std::endl;
    std::cout << "port_num :" << ip_port.second << "." << std::endl;
    while (i < server_name.size())
    {
        std::cout << "server name :" << server_name[i] << "." << std::endl;
        i++;
    }
    std::cout << "root :" << path_server_root << "." << std::endl;
    std::cout << "index :" << path_server_index << "." << std::endl;
    std::cout << "client_max_body_size :" << client_max_body_size << "." << std::endl;
    i = 0;
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

Vector_str ServerConfig::ft_splitv2(std::string str, char c)
{
    Vector_str re;
    size_t pos = str.find(c);
    size_t to = 0;
    while (pos != std::string::npos)
    {
        re.push_back(str.substr(to, pos - to));
        to = pos + 1;
        pos = str.find(c, to);
    }
    if (to != std::string::npos)
      re.push_back(str.substr(to));

    return re;
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

LocationConfig ServerConfig::get_conf(std::string path)
{
    return get_Location_Config(path);
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
