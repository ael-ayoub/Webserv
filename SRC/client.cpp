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
            throw(std::runtime_error("Error sending response to client"));
        if (sent == 0)
            throw(std::runtime_error("Client disconnected before full response was sent"));
        total_sent += sent;
    }
}

std::string _getHeader(int fd_client)
{
    std::string header;
    while (true)
    {
        std::string line = ft_getline(fd_client);
        // std::cout << line << " NOOOOOOO\n";
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

std::string extract_cookie_username(const std::string &request_string)
{
    std::istringstream stream(request_string);
    std::string line;

    while (std::getline(stream, line))
    {
        if (!line.empty() && line[line.length() - 1] == '\r')
        {
            line.erase(line.length() - 1);
        }

        if (line.find("Cookie:") == 0)
        {
            size_t pos = line.find("username=");
            if (pos != std::string::npos)
            {
                pos += 9;
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


#define HEADER_SIZE 1024 * 16

void Socket::HandleClient(int fd_client, Config &a, std::map<int, ClientState> &status)
{
    std::vector<ServerConfig> servers = a.get_allserver_config();
    std::pair<std::string, int> ip_port;
    Request test_request;
    std::string response;

    ClientState &state = status[fd_client];

    if (!state.complete_header)
    {
        std::cout << "Reading header from fd: " << fd_client << std::endl;
        char buffer[4096];
        ssize_t n = read(fd_client, buffer, sizeof(buffer));
        if (n == 0)
        {
            std::cout << "Client disconnected fd: " << fd_client << std::endl;
            state.close = true;
            state.cleanup = true;
            state.send_data = false;
            state.waiting = false;
            return;
        }
        else if (n < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                std::cout << "No data available to read from fd: " << fd_client << " (EAGAIN/EWOULDBLOCK)" << std::endl;
                return;
            }
            std::cout << "Error reading from fd: " << fd_client << ", errno: " << errno << std::endl;
            state.close = true;
            state.cleanup = true;
            state.send_data = false;
            return;
        }
        else
        {
            std::cout << "Read " << n << " bytes from fd: " << fd_client << std::endl;
            try{
                std::cout << "Appending " << n << " bytes to readstring for fd: " << fd_client << std::endl;
                state.readstring += std::string(buffer, n);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                state.response = ErrorResponse::Error_BadRequest(a);
                state.close = true;
                state.cleanup = true;
                state.send_data = true;
                return;
            }

            std::cout << "Current readstring size for fd " << fd_client << " is: " << state.readstring.size() << " bytes." << std::endl;
            size_t pos = state.readstring.find("\r\n\r\n");
            if (pos != std::string::npos)
            {
                std::cout << "Complete header received from fd: " << fd_client << std::endl;
                state.header = state.readstring.substr(0, pos + 4);
                std::cout << "Header size for fd " << fd_client << " is: " << state.header.size() << " bytes." << std::endl;
                if (state.header.size() > HEADER_SIZE)
                {
                    state.response = ErrorResponse::Error_BadRequest(a);
                    state.close = true;
                    state.cleanup = true;
                    state.send_data = true;
                    return;
                }
                state.complete_header = true;
                std::cout << "remove header from readstring for fd: " << fd_client << std::endl;
                state.readstring.erase(0, pos + 4);
                std::cout << "after erasing, readstring size for fd " << fd_client << " is: " << state.readstring.size() << std::endl;
                test_request.parse_request((char *)state.header.c_str(), a);
                state.method = test_request.get_method();
                state.path = test_request.get_path();
            }
            else
            {
                std::cout << "Incomplete header, waiting for more data from fd: " << fd_client << std::endl;
                state.waiting = true;
                return;
            }
        }
    }

    if (!state.complete_header || state.header.empty())
    {
        state.response = ErrorResponse::Error_BadRequest(a);
        state.close = true;
        state.cleanup = true;
        state.send_data = true;
        return;
    }

    // if (state.path == "/uploads")
    // {

    //     size_t pos = state.readstring.find("\r\n\r\n");
    //     if (pos == std::string::npos)
    //     {
    //         char buffer[4096];
    //         ssize_t n = read(fd_client, buffer, sizeof(buffer));
    //         if (n > 0)
    //         {
    //             state.readstring.append(buffer);
    //             if (state.readstring.find("\r\n\r\n") == std::string::npos)
    //             {
    //                 // generate bad request and return
    //                 response = ErrorResponse::Error_BadRequest(a);
    //                 _sendReaponse(response, fd_client);
    //                 // state.close = true;
    //                 return;
    //             }
    //         }
    //         else
    //         {
    //             if (errno == EAGAIN || errno == EWOULDBLOCK)
    //                 return;
    //             else
    //             {
    //                 response = ErrorResponse::Error_BadRequest(a);
    //                 _sendReaponse(response, fd_client);

    //                 return;
    //             }
    //             // state.close = true;
    //         }
    //     }
    //     else
    //     {
    //         state.metadata = state.readstring.substr(0, pos + 4);
    //         state.readstring.erase(0, pos + 4);
    //         state.complete_metadata = true;
    //     }
    // }

    // else if (state.path == "/login")
    // {
    //     state.metadata = state.readstring;
    //     state.readstring.erase(state.readstring.size());
    // }


    std::cout << "########### [..] handle method: " << state.method << " for fd: " << fd_client << std::endl;
    if (state.method == "GET" || state.method == "DELETE")
    {
        // handle the request for GET ...
        try
        {
            state.response = m.GetMethod(a, test_request, servers[0]);
            state.send_data = true;
            state.cleanup = true;

            if (state.header.find("Connection: close") != std::string::npos)
            {
                std::cout << "the client send Connection: close header, so we close the connection after response.\n";
                state.close = true;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }
    else if (state.method == "POST")
    {
        if (state.path == "/uploads" && !state.complete_upload)
        {
            std::cout << "-----------------------------------------------------\n";
            std::cout << state.readstring << std::endl;
            std::cout << "-----------------------------------------------------\n";
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
            state.response = PostSession(UserName);
        }
    }
    else 
    {
        state.response = ErrorResponse::Error_BadRequest(a);
        state.close = true;
        state.cleanup = true;
        state.send_data = true;
    }
}
