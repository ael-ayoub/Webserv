#include "../../INCLUDES/Request.hpp"

int Request::get_port()
{
    return port;
}

std::string Request::get_Hostname()
{
    return hostname;
}

std::string Request::check_requestline(std::string request_line, Config a)
{
    int spaces = 0;
    int newline = 0;
    int car = 0;
    size_t i = 0;
    // std::cout << "requ : " << request_line << std::endl;
    while (request_line[i])
    {
        // if (i == request_line.size())
        //     std::cout << "int: " << static_cast<int>(request_line[i]) << std::endl;
        if (request_line[i] == ' ')
            spaces++;
        if (request_line.size() > 5 && i == request_line.size() - 1 && request_line[i] == '\n')
            newline++;
        if (request_line.size() > 5 && i == request_line.size() - 2 && request_line[i] == '\r')
            car++;
        i++;
    }
    if (spaces != 2 || newline != 1 || car != 1)
    {
        return ErrorResponse::Error_BadRequest(a);
    }
    Vector_str args = ServerConfig::ft_splitv2(request_line, ' ');
    if (args.size() != 3)
        return ErrorResponse::Error_BadRequest(a);
    if (args[0] != "GET" && args[0] != "POST" && args[0] != "DELETE")
        return ErrorResponse::Error_MethodeNotAllowed(a); // 405
    if (args[1][0] != '/')
        return ErrorResponse::Error_BadRequest(a);
    if (args[2] != "HTTP/1.1\r\n" && args[2] != "HTTP/1.0\r\n")
        return ErrorResponse::Error_BadRequest(a);
    method = args[0];
    path = args[1];
    HTTP = args[2];
    return "NONE";
}

bool check_ip(std::string info)
{
    int num;
    Vector_str ip_port = ServerConfig::ft_splitv2(info, '.');
    if (ip_port.size() != 4)
        throw Config::ErrorSyntax();
    // std::cout << "size is " << ip_port.size() << "\n";
    size_t j = 0;
    while (j < ip_port.size())
    {
        syntax_server::check_number(ip_port[j]);
        std::istringstream(ip_port[j]) >> num;
        if (num < 0 || num > 255)
            return false;
        j++;
    }
    return true;
}

std::string Request::check_headerline(std::string header_line, Config a)
{
    int spaces = 0;
    int newline = 0;
    int car = 0;
    size_t i = 0;
    // std::cout << "str: " << header_line << std::endl;
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
        // std::cout << "spa " << spaces << newline << car << "\n";
        return ErrorResponse::Error_BadRequest(a);
    }
    Vector_str args = ServerConfig::ft_splitv2(header_line, ' ');
    if (args.size() != 2)
        return ErrorResponse::Error_BadRequest(a); // 400
    if (args[0] != "Host:")
        return ErrorResponse::Error_BadRequest(a); // 400

    Vector_str ip_port = ServerConfig::ft_splitv2(args[1], ':');
    if (ip_port.size() != 2)
        return ErrorResponse::Error_BadRequest(a); // 400
    if (ip_port[0] != "localhost" && check_ip(ip_port[0]) == false)
        return ErrorResponse::Error_BadRequest(a); // 400
    int start = ip_port[1].find('\r');
    std::string ip = ip_port[1].substr(0, start);
    int v_ip;
    std::istringstream ss(ip);
    ss >> v_ip;
    if (v_ip < 0 || v_ip > 65535)
        return ErrorResponse::Error_BadRequest(a);
    if (ip_port[1][start] != '\r' && ip_port[1][start + 1] != '\r')
        return ErrorResponse::Error_BadRequest(a);
    port = v_ip;
    hostname = ip_port[0];
    if (hostname == "localhost")
        hostname = "127.0.0.1";
    return "NONE";
}
\


std::string Request::check_request(std::string str, Config a)
{
    // size_t i = 0;
    int from = 0;
    // bool request = false;
    std::string request_line;

    // bool header = false;
    std::string header_line;

    int first = str.find('\n');
    
    request_line = str.substr(0, first + 1);
    from = first + 1;
    first = str.find('\n', from);
    header_line = str.substr(from, first - from + 1);

    std::string response;
    response = check_requestline(request_line, a);
    if (response != "NONE")
        return response;
    response = check_headerline(header_line, a);
    if (response != "NONE")
        return response;
    return "NONE";
}

std::string Request::parse_request(char *buffer, Config a)
{
    std::string str(buffer);
    std::string response = check_request(str, a);
    if (response != "NONE")
        return response;
    return "NONE";
}

std::string Request::get_method()
{
    return method;
}

std::string Request::get_path()
{
    return path;
}


