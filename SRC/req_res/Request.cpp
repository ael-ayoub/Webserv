#include "../../INCLUDES/Request.hpp"

int hex_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F')
        return 10 + (c - 'A');
    return -1;
}

bool url_decode_path(const std::string &in, std::string &out)
{
    out.clear();
    out.reserve(in.size());
    for (size_t i = 0; i < in.size(); i++)
    {
        if (in[i] != '%')
        {
            out += in[i];
            continue;
        }
        if (i + 2 >= in.size())
            return false;
        int j = hex_value(in[i + 1]);
        int k = hex_value(in[i + 2]);
        if (j < 0 || k < 0)
            return false;
        unsigned char decoded = (unsigned char)((j << 4) | k);
        if (decoded == '?')
        {
            out += '%';
            out += in[i + 1];
            out += in[i + 2];
        }
        else if (decoded == '\0')
        {
            return false;
        }
        else
        {
            out += (char)decoded;
        }
        i += 2;
    }
    return true;
}

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
    {
        return ErrorResponse::Error_BadRequest(a);
    }
    Vector_str args = ServerConfig::ft_splitv2(request_line, ' ');
    if (args.size() != 3)
        return ErrorResponse::Error_BadRequest(a);
    // std::cout << "first arg is " << args[0] << std::endl;
    if (args[0] != "GET" && args[0] != "POST" && args[0] != "DELETE")
    {
        return ErrorResponse::Error_BadRequest(a); // 405
    }
    if (args[1][0] != '/')
        return ErrorResponse::Error_BadRequest(a);
    if (args[2] != "HTTP/1.1\r\n" && args[2] != "HTTP/1.0\r\n")
        return ErrorResponse::Error_BadRequest(a);
    method = args[0];

    std::string decoded;
    if (!url_decode_path(args[1], decoded))
        return ErrorResponse::Error_BadRequest(a);
    path = decoded;

    HTTP = args[2];
    return "NONE";
}

bool check_ip(std::string info)
{
    int num;
    Vector_str ip_port = ServerConfig::ft_splitv2(info, '.');
    if (ip_port.size() != 4)
        return false;
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

bool CheckNumber(std::string str)
{
    int j = 0;
    while (str[j])
    {
        // std::cout << "im in " << str[j] << "\n";
        if (!std::isdigit(str[j]))
            return true;
        j++;
    }
    return false;
}

std::string Request::check_headerline(std::string header_line, Config &a)
{
    int spaces = 0;
    int newline = 0;
    int car = 0;
    size_t i = 0;

    // std::cout << "-------------str: " << header_line << std::endl;
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
    {
        // std::cout << "Errorsadas\n";
        return ErrorResponse::Error_BadRequest(a); // 400
    }
    // std::cout << "before\n";
    int start = ip_port[1].find('\r');
    // if (ip_port[1][start + 1] != '\n')
    //     return ErrorResponse::Error_BadRequest(a);
    // std::cout << "after\n";
    std::string ip = ip_port[1].substr(0, start);
    if (CheckNumber(ip) == true)
        return ErrorResponse::Error_BadRequest(a);
    int v_ip;
    std::istringstream ss(ip);
    ss >> v_ip;
    if (v_ip < 0 || v_ip > 65535)
        return ErrorResponse::Error_BadRequest(a);
    port = v_ip;
    hostname = ip_port[0];
    if (hostname == "localhost")
        hostname = "127.0.0.1";
    return "NONE";
}

std::string CheckContentLenght(std::string str, Config a)
{
    // ServerConfig x;
    size_t num;

    for (size_t i = 0; i < str.size() - 2; i++)
    {
        if (!isdigit(str[i]))
        {
            // std::cout << str[i] << ".\n";
            return ErrorResponse::Error_BadRequest(a);
        }
    }

    std::stringstream s(str);
    s >> num;
    if (ServerConfig::CheckClientMaxBodySize(num) == true)
        return ErrorResponse::Error_PayloadTooLarge(a);
    return "NONE";
}

bool CheckContentType(std::vector<std::string> &str)
{
    // std::cout << "first str is " << str[1] << ", the seconde is " << str[2] << std::endl;
    if (str[1] != "multipart/form-data;")
        return true;
    std::pair<std::string, std::string> tmp = ServerConfig::ft_splito(str[2], '=');
    if (tmp.first != "boundary")
        return true;
        //save the data
    return false;
}

std::string Request::check_request(std::string str, Config a)
{
    std::vector<std::string> args;

    std::string tmp;
    for (size_t b = 0; b < str.size(); b++)
    {
        if (str[b] != '\n')
        {
            // if (str[b] == '\r')
            //     std::cout << "YYYYYYYYYYEEEEEEs\n";
            tmp += str[b];
            // std::cout << "tmp is : " << tmp << ", and b is : " << b << std::endl;
        }
        else if (str[b] == '\n')
        {
            tmp += '\n';
            args.push_back(tmp);
            // std::cout << "str is : " << tmp << std::endl;
            // std::cout << "b is : " << b << ", size total is : " << str.size() << std::endl;
            // if (str.size() - 1 == b)
            // {
            // }
            tmp.clear();
        }
        if (b + 1 == str.size())
        {
            tmp += '\n';
            args.push_back(tmp);
            tmp.clear();

        }
    }
    
    // std::cout << "tmp: " << tmp <<std::endl;
    // std::cout << "lenght first one is : " << args[0].size() << std::endl;
    // std::cout << "lenght seconde one is : " << tmp.size() << std::endl;
    // for (size_t b = 0; b < args.size(); b++)
    // {
    //     std::cout << b << ": " << args[b] << std::endl;
    // }
    std::string response;
    response = check_requestline(args[0], a);
    if (response != "NONE")
    {
        return response;
    }
    // std::cout << "first has : " << args[0].size() << std::endl;
    // std::cout << " sec: " << args[1].size() << std::endl;
    // std::cout << "NONEEEEEEEE" << std::endl;
    // std::cout << args[0] << std::endl;
    response = check_headerline(args[1], a);
    if (response != "NONE")
    {
        // std::cout << "seconde err\n";
        return response;
    }
    // std::cout << "been here\n";
    // bool lenght = false;
    // bool type = false;
    std::string res;
    for (size_t b = 2; b < args.size() && get_method() == "POST"; b++)
    {
        std::vector<std::string> tmp = ServerConfig::ft_splitv2(args[b], ' ');
        // std::cout << tmp[0] << std::endl;
        // (tmp[0] == "Content-Type:" && tmp.size() != 3) // check thsi after
        if ((tmp[tmp.size() - 1][tmp[tmp.size() - 1].size() - 1] != '\n')
            || (tmp[tmp.size() - 1][tmp[tmp.size() - 1].size() - 2] != '\r'))
        {
            // std::cout << "dsadadasdadas\n\n";
            return ErrorResponse::Error_BadRequest(a);
        }

        if (tmp[0] == "Content-Length:")
        {
            // lenght = true;
            res = CheckContentLenght(tmp[1], a);
            content_length = tmp[1];
            if (res != "NONE")
            {
                // std::cout << "He pass the test\n";
                return res;
            }
        }
        // if (tmp[0] == "Content-Type:")
        // {
        //     type = true;
        //     if (CheckContentType(tmp) == true)
        //     {
        //         std::cout << "The seconde cause\n";
        //         return ErrorResponse::Error_BadRequest(a);
        //     }
        //     // std::cout << "it is content type\n";
        // }
    }

    // if ((type == false || lenght == false) && get_method() == "POST")
    // {
    //     std::cout << "here\n";
    //     return ErrorResponse::Error_BadRequest(a);
    // }
    // std::cout << "i have this : " << str  << std::endl;
    // if (str.substr(str.length() - 1) != "\r\n")
    //     return ErrorResponse::Error_BadRequest(a);
    return "NONE";
}

std::string Request::parse_request(std::string buffer, Config a)
{
    // std::string str(buffer);
    std::string response = check_request(buffer, a);
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



size_t Request::get_content_length()
{
    if (content_length == "NONE")
        return 0;
    size_t num;
    std::istringstream ss(content_length);
    ss >> num;
    return num;
}