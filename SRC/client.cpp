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
        // std::cout
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

int i = 0;

void Socket::HandleClient(int fd_client, Config &a, std::map<int, ClientState> &status)
{

    std::vector<ServerConfig> servers = a.get_allserver_config();
    std::pair<std::string, int> ip_port;
    Request test_request;
    std::string response;

    ClientState &state = status[fd_client];

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
   
    
    if (state.method == "POST" && !state.complete_metadata)
    {
        if (state.path == "/uploads")
            state.metadata = _getMetadata(fd_client);
        else
        {
            char buffer[1000];
            size_t b = read(fd_client, buffer, 1000);
            if (b < 0)
                return;
            buffer[b] = '\0';
            state.metadata = std::string(buffer, b);
        }
        if (state.metadata.empty())
            return;
        state.complete_metadata = true;
    }

    std::cout << state.header << std::endl;
    std::cout << state.metadata << std::endl;

    if (state.method == "GET")
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
    else if (state.method == "DELETE")
    {
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
    }

    if (!response.empty())
    {
        _sendReaponse(response, fd_client);
    }
}
