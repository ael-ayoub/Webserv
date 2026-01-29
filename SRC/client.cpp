#include "../INCLUDES/Webserv.hpp"

std::string ft_getline(int fd)
{
    if (fd < 0)
        return "";

    char ch;
    std::string line;

    while (true)
    {
        int byte_read = read(fd, &ch, 1);
        if (byte_read < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return "EWOULDBLOCK";
            }
            std::cout << "Error happen Try to Read Request !!\n";
            return "ERROR";
        }
        if (byte_read == 0 || ch == '\n')
            break;
        line += ch;
    }
    return line;
}

void _sendReaponse(const std::string &response, int fd_client)
{
    size_t total_sent = 0;
    while (total_sent < response.size())
    {
        ssize_t sent = send(fd_client, response.c_str() + total_sent,
                            response.size() - total_sent, 0);
        if (sent < 0)
        {
            perror("send error");
            break;
        }
        if (sent == 0)
        {
            std::cerr << "Connection closed by client" << std::endl;
            break;
        }
        total_sent += sent;
    }
}

std::string _getHeader(int fd_client)
{
    std::string header;
    while (true)
    {
        std::string line = ft_getline(fd_client);
        if (line == "EWOULDBLOCK" || line == "ERROR")
            return "";
        if (line == "\r" || line.empty())
            break;
        header += line + "\n";
    }
    return header;
}

std::string _getMetadata(int fd_client)
{
    std::string metadata;
    while (true)
    {
        std::string line = ft_getline(fd_client);
        if (line == "EWOULDBLOCK")
            return "";
        if (line == "\r" || line.empty())
            break;
        metadata += line + "\n";
    }
    return metadata;
}

// int i = 0;

std::string PostSession(std::string username)
{
    std::string body = "Session OK";

    std::string response;
    response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/plain\r\n";
    response += "Set-Cookie: username=" + username + "\r\n";
    response += "Content-Length: 10\r\n";
    response += "\r\n";
    response += body;

    return response;
}
std::string CheckSession(std::string message)
{
    std::string body = message;

    std::string response;
    response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/plain\r\n";
    std::stringstream ss;
    ss << body.size();
    response += "Content-Length: " + ss.str() + "\r\n";
    response += "\r\n";
    response += body;

    return response;
}

std::string FirstLineHeader(std::string FirstLine)
{
    size_t i = 0;
    std::string tmp;
    while (i < FirstLine.size())
    {
        if (FirstLine[i] != '\r')
            tmp += FirstLine[i];
        else
            break;
        i++;
    }
    return tmp;
}

std::string LastLine(std::string LastLine)
{
    std::vector<std::string> args;

    std::string tmp;
    for (size_t b = 0; b < LastLine.size(); b++)
    {
        if (LastLine[b] != '\n')
            tmp += LastLine[b];
        else
        {
            tmp += '\n';
            args.push_back(tmp);
            tmp.clear();
        }
    }
    return args[args.size() - 1];
}

/////////////////////////////////////////////////////////////
#include <iostream>
#include <string>
#include <sstream>

// Extract query parameter 'name' from request start line
std::string extract_name(const std::string &request_string)
{
    std::istringstream stream(request_string);
    std::string line;

    // Read first line (start line)
    if (std::getline(stream, line))
    {
        // Find "?name=" in the start line
        size_t pos = line.find("?name=");
        if (pos != std::string::npos)
        {
            pos += 6; // Move past "?name="
            size_t end = line.find_first_of(" &", pos);
            if (end != std::string::npos)
            {
                return line.substr(pos, end - pos);
            }
            else
            {
                return line.substr(pos);
            }
        }
    }

    return ""; // Not found
}

// Extract 'username' cookie from Cookie header
std::string extract_cookie_username(const std::string &request_string)
{
    std::istringstream stream(request_string);
    std::string line;

    // Read all lines to find Cookie header
    while (std::getline(stream, line))
    {
        // Remove carriage return if present (C++98 compatible)
        if (!line.empty() && line[line.length() - 1] == '\r')
        {
            line.erase(line.length() - 1);
        }

        if (line.find("Cookie:") == 0)
        {
            // Extract username cookie value
            size_t pos = line.find("username=");
            if (pos != std::string::npos)
            {
                pos += 9; // Move past "username="
                size_t end = line.find_first_of(";\r\n", pos);
                if (end != std::string::npos)
                {
                    return line.substr(pos, end - pos);
                }
                else
                {
                    return line.substr(pos);
                }
            }
        }
    }

    return "";
}

/////////////////////////////////////////////////////////////

// void Socket::HandleClient(int fd_client, Config &a, std::map<int, ClientState> &status)
// {
//     std::vector<ServerConfig> servers = a.get_allserver_config();
//     std::pair<std::string, int> ip_port;
//     Request test_request;
//     std::string response;

//     ClientState &state = status[fd_client];

//     if (!state.complete_header)
//     {
//         state.header = _getHeader(fd_client);
//         if (!state.header.empty())
//         {
//             state.complete_header = true;
//             test_request.parse_request((char *)state.header.c_str(), a);
//             state.method = test_request.get_method();
//             state.path = test_request.get_path();
//         }
//         else
//         {
//             // response = ErrorResponse::Error_BadRequest(a);
//             // _sendReaponse(response, fd_client);
//             return;
//         }
//     }

//     // std::cout << "Header received:\n" << state.header << std::endl;
//     // std::cout << "Method: " << state.method << ", Path: " << state.path << std::endl;

//     if (state.method == "POST")
//     {
//         state.expected_content_length = test_request.get_content_length();
//         std::cout << "Expected Content-Length: " << state.expected_content_length << std::endl;
//         if (state.expected_content_length <= 1)
//         {
//             response = ErrorResponse::Error_BadRequest(a);
//             _sendReaponse(response, fd_client);
//             status.erase(fd_client);
//             close(fd_client);
//             return;
//         }
//         if (!state.complete_metadata)
//             state.metadata = _getMetadata(fd_client);
//         if (state.metadata.empty())
//             return;
//         std::cout << "Metadata received:\n" << state.metadata << std::endl;
//         state.complete_metadata = true;

//         if (state.path == "/uploads")
//         {
//             if (!state.complete_upload)
//             {
//                 if (_uploadFile(fd_client, state) == false)
//                     return;
//             }
//             state.complete_upload = true;
//             response = generateSuccessMsg();
//             std::cout << "Upload complete, sending response.\n";
//         }
//         else if (state.path == "/login")
//         {
//             std::cout << "Processing /login\n";
//             std::string UserName;
//             std::vector<std::string> tmp = ServerConfig::ft_splitv2(state.metadata, '=');
//             if (tmp.size() != 2)
//                 return;
//             UserName = tmp[1];
//             std::cout << "Extracted username: " << UserName << std::endl;
//             response = PostSession(UserName);
//         }
//     }
//     else if (state.method == "GET" || state.method == "DELETE")
//     {

//         std::string name = extract_name(state.header);
//         std::string cookie = extract_cookie_username(state.header);

//         if (!name.empty() && !cookie.empty())
//         {
//             std::cout << "name: " << name << ", cookie: " << cookie << std::endl;
//             if (cookie == name)
//                 response = CheckSession("FOOUND!");
//             else
//                 response = CheckSession("NOT FOUND!");
//         }
//         else
//         {
//             Config a;
//             size_t i = 0;
//             response = test_request.parse_request(state.header, a);
//             while (i < servers.size() && response == "NONE")
//             {
//                 ip_port = servers[i].get_ip();
//                 if (ip_port.first == test_request.get_Hostname() && ip_port.second == test_request.get_port())
//                 {
//                     response = m.GetMethod(a, test_request, servers[i]);
//                     break;
//                 }
//                 i++;
//             }
//         }
//     }
//     if (response.empty() || response == "NONE")
//     {
//         response = ErrorResponse::Error_BadRequest(a);
//     }
//     if (!response.empty())
//     {
//         _sendReaponse(response, fd_client);
//         status.erase(fd_client);
//         close(fd_client);
//     }
// }

/*
what is the bihavior of this functoins ??


handle client good ,

the first thing is:
1- read header
2- parse header
3- check if method is post
4- read metadata
5- check if path is /uploads
6- upload file
7- generate success message


what is cases that i have ??
- if header is not complete
- if metadata is not complete
- if upload is not complete
- if method is get or delete

i have get method
- extract name from request line
- extract cookie from header
- compare name and cookie
- generate response

i have post method
- if path is /uploads
    - upload file
    - generate success message
- if path is /login
    - extract username from metadata
    - generate session response

in case of post with uploads
- read metadata
- check if metadata is complete
- upload file
- generate success message

else
- wait for complete data
- generate bad request


i have delete method
- not implemented yet


when can i start first ??

- when header is complete
- when metadata is complete
- when upload is complete



*/

void Socket::HandleClient(int fd_client, Config &a, std::map<int, ClientState> &status)
{
    std::vector<ServerConfig> servers = a.get_allserver_config();
    std::pair<std::string, int> ip_port;
    Request test_request;
    std::string response;

    ClientState &state = status[fd_client];
    // printCurrentTime(); 
    // printf ("-------"*120);
    // std::cout << "-----------------------------------------------------------" << std::endl;
    // std::cout << Request << std::endl;
    // std::cout << "-----------------------------------------------------------" << std::endl;
    if (!state.complete_header)
    {
        state.header = _getHeader(fd_client);
        if (!state.header.empty())
        {
            state.complete_header = true;
            test_request.parse_request((char *)state.header.c_str(), a);
            state.method = test_request.get_method();
            state.path = test_request.get_path();
        }
    }

    if (!state.complete_header)
        return;
    std::cout << "----------------------------------------------------------------------" << std::endl;
    std::cout << state.header << std::endl;
    std::cout << "----------------------------------------------------------------------" << std::endl;
    if (state.method == "POST" && !state.complete_metadata)
    {
        if (state.path == "/uploads")
            state.metadata = _getMetadata(fd_client);
        else
        {
            char buffer[1000];
            ssize_t b = read(fd_client, buffer, 1000);
            if (b < 0)
                return;
            buffer[b] = '\0';
            state.metadata = std::string(buffer, b);
        }
        if (state.metadata.empty())
            return;
        state.complete_metadata = true;
    }


    if (state.method == "GET" || state.method == "DELETE")
    {
        std::string name = extract_name(state.header);
        std::string cookie = extract_cookie_username(state.header);

        if (!name.empty() && !cookie.empty())
        {
            std::cout << "name: " << name << ", cookie: " << cookie << std::endl;
            if (cookie == name)
                response = CheckSession("FOOUND!");
            else
                response = CheckSession("NOT FOUND!");
        }
        else 
        {
            size_t i = 0;
            response = test_request.parse_request((char *)state.header.c_str(), a);
            while (i < servers.size())
            {
                ip_port = servers[i].get_ip();
                if (ip_port.first == test_request.get_Hostname() && ip_port.second == test_request.get_port())
                break;
                i++;
            }
            if (response == "NONE")
            response = m.GetMethod(a, test_request, servers[i]);
        }
    }
    else if (state.method == "POST")
    {
        if (state.path == "/uploads" && !state.complete_upload)
        {
            if (_uploadFile(fd_client, state) == false)
                return;
            state.complete_upload = true;
            response = generateSuccessMsg();
        }
        else if (state.path == "/login")
        {
            std::string UserName;
            std::vector<std::string> tmp = ServerConfig::ft_splitv2(state.metadata, '=');
            if (tmp.size() != 2)
                return;
            UserName = tmp[1];
            response = PostSession(UserName);
        }
    }

    if (!response.empty())
    {
        _sendReaponse(response, fd_client);
    }
}
