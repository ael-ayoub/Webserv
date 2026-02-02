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

// std::string extract_cookie_username(const std::string &request_string)
// {
//     std::istringstream stream(request_string);
//     std::string line;

//     while (std::getline(stream, line))
//     {
//         if (!line.empty() && line[line.length() - 1] == '\r')
//         {
//             line.erase(line.length() - 1);
//         }

//         if (line.find("Cookie:") == 0)
//         {
//             size_t pos = line.find("username=");
//             if (pos != std::string::npos)
//             {
//                 pos += 9;
//                 size_t end = line.find_first_of(";\r\n", pos);
//                 if (end != std::string::npos)
//                 {
//                     return line.substr(pos, end - pos);
//                 }
//                 else
//                 {
//                     return line.substr(pos);
//                 }
//             }
//         }
//     }

//     return "";
// }

unsigned long long get_current_timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000ULL) + (tv.tv_usec / 1000ULL);
}

bool check_timeout(unsigned long long timestamp, unsigned long long timeout)
{
    // sleep(1);
    unsigned long long current_time = get_current_timestamp();
    std::cout << "Checking timeout: current_time = " << current_time << ", timestamp = " << timestamp << ", timeout = " << timeout << std::endl;
    std::cout << "Time elapsed: " << (current_time - timestamp) << " ms" << std::endl;
    return (current_time - timestamp) <= timeout;
}

void cloce_connection(ClientState &state)
{
    state.close = true;
    state.cleanup = true;
    state.send_data = false;
    state.waiting = false;
}

void Socket::HandleClient(int fd_client, Config &a, std::map<int, ClientState> &status)
{
    std::vector<ServerConfig> servers = a.get_allserver_config();
    std::pair<std::string, int> ip_port;
    Request request;

    ClientState &state = status[fd_client];
    state.timestamp = get_current_timestamp();
    if (!state.complete_header)
    {
        std::cout << "Starting to read header from fd: " << fd_client << std::endl;
        if (!_parse_header(state, fd_client, request, a))
            return;
    }

    if (state.method == "GET" || state.method == "DELETE")
    {
        if (!_process_get_delete_request(fd_client, state, request, a, servers, m))
            return;
    }
    else if (state.method == "POST")
    {
        if (!state.complete_metadata)
        {
            if (!_parse_metadata(state, fd_client, a))
                return;
        }
        if (state.complete_metadata)
        {
            std::cout << "############ [..] handle POST method for fd: " << fd_client << std::endl;
            // Handle POST request here
            if (!_process_post_request(fd_client, state, request, a, servers, m))
                return;
        }
        else
        {
            state.response = ErrorResponse::Error_BadRequest(a);
            state.close = true;
            state.cleanup = true;
            state.send_data = true;
        }
    }
}