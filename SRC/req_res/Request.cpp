#include "../../INCLUDES/Request.hpp"

bool Request::check_requestline(std::string request_line)
{
    int spaces = 0;
    int newline = 0;
    int car = 0;
    size_t i = 0;
    while (request_line[i])
    {
        if (request_line[i] == ' ')
            spaces++;
        if (request_line.size() > 5 && i == request_line.size() - 1 && request_line[i] == '\n')
            newline++;
        if (request_line.size() > 5 && i == request_line.size() - 2 && request_line[i] == '\r')
            car++;
        i++;
    }
    if (spaces != 2 || newline != 1 || car != 1)
        return false;
    Vector_str args = ServerConfig::ft_splitv2(request_line, ' ');
    if (args.size() != 3)
        return false;
    if (args[0] != "GET" && args[0] != "POST" && args[0] != "DELETE")
        return false; //405
    if (args[1][0] != '/')
        return false;
    if (args[2] != "HTTP/1.1\r\n" && args[2] != "HTTP/1.0\r\n")
        return false; //505
    return true;
}

bool Request::check_headerline(std::string header_line)
{
    int spaces = 0;
    int newline = 0;
    int car = 0;
    size_t i = 0;
    std::cout << "str: " << header_line << "." << std::endl;
    while (header_line[i])
    {
        if (header_line[i] == ' ')
            spaces++;
        if (header_line.size() > 5 && i == header_line.size() - 1 && header_line[i] == '\n')
            newline++;
        if (header_line.size() > 5 && i == header_line.size() - 2 && header_line[i] == '\r')
            car++;
        i++;
    }
    if (spaces != 1 || newline != 1 || car != 1)
    {
        std::cout << "spa " << spaces << newline << car << "\n";
        return false;
    }
    Vector_str args = ServerConfig::ft_splitv2(header_line, ' ');
    if (args.size() != 2)
        return false;//400
    if (args[0] != "Host:")
        return false; //400

    Vector_str ip_port = ServerConfig::ft_splitv2(args[1], ':');
    if (ip_port.size() != 2)
        return false; //400
    if (ip_port[0] != "localhost" && ip_port[0] != "127.0.0.1")
        return false;//400
    int start = ip_port[1].find('\r');
    std::string ip = ip_port[1].substr(0, start);
    std::cout << "ip:   " << ip << std::endl;
    return true;
}

bool Request::check_request(std::string str)
{
    // size_t i = 0;
    int from = 0;
    // bool request = false;
    std::string request_line;

    // bool header = false;
    std::string header_line;

    // while (str[i])
            int first = str.find('\n');
            request_line = str.substr(0, first + 1);
            from = first + 1;
            first = str.find(from, '\n');
            header_line = str.substr(from, first - from);
            //////////// header_line taycoper tallekher 
    if (check_requestline(request_line) == false)
        return false;
    if (check_headerline(header_line) == false)
        return false;
    return true;
}

void Request::parse_request(char *buffer)
{
    int i = 0;
    std::string str(buffer);
    if (check_request(str) == false)
        return ;
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
