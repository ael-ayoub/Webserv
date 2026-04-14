#include "../includes/Webserv.hpp"


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


unsigned long long get_current_timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000ULL) + (tv.tv_usec / 1000ULL);
}

bool times_out(unsigned long long timestamp, unsigned long long timeout)
{
    unsigned long long current_time = get_current_timestamp();
    return (current_time - timestamp) <= timeout;
}

void cloce_connection(ClientState &state)
{
    state.close = true;
    state.cleanup = true;
    state.send_data = false;
    state.waiting = false;
}

ServerConfig checkRightServer(std::vector<ServerConfig> servers, std::map<int, 
                            ClientState> &status, int fd_client, Request request)
{
    std::vector<ServerConfig> tmp;
    size_t p = 0;
    while (p < servers.size())
    {
        std::vector<std::pair<std::string, int> > ip_port = servers[p].get_ip();
        size_t o = 0;
        while (o < ip_port.size())
        {
            if (ip_port[o].second == status[fd_client].port)
            {
                tmp.push_back(servers[p]);
                break;
            }
            o++;
        }
        p++;
    }

    p = 0;
    while (p < tmp.size())
    {
        Vector_str tmp_serverNames = tmp[p].get_server_name();
        size_t i = 0;
        while (i < tmp_serverNames.size())
        {
            if (request.get_Host() == tmp_serverNames[i])
            {
                return tmp[p];
            }
            i++;
        }
        p++;
    }

    if (tmp.size() == 0)
        return ServerConfig();
    return tmp[0];
}

void Socket::HandleClient(int fd_client, Config &a, std::map<int, ClientState> &status)
{
    std::pair<std::string, int> ip_port;
    Request request;

    std::vector<ServerConfig> servers = a.get_allserver_config();
    if (servers.empty())
    return;
    
    ClientState &state = status[fd_client];
    if (state.send_data)
        return;

    state.timestamp = get_current_timestamp();

    if (state.complete_header)
    {
        servers[0] = checkRightServer(servers, status, fd_client, request);
        ServerConfig::client_max_body_size = servers[0].get_client_max_body_size();
        std::string parse_res = request.parse_request(state.header, a);
        if (parse_res != "NONE")
        {
            state.response = parse_res;
            state.close = true;
            state.cleanup = true;
            state.send_data = true;
            return;
        }
    }
    
    if (!state.complete_header)
    {
        if (!_parse_header(state, fd_client, request, a))
            return;
    }
    state.timestamp = get_current_timestamp();

    servers[0] = checkRightServer(servers, status, fd_client, request);
    ServerConfig::client_max_body_size = servers[0].get_client_max_body_size();

    if (state.method == "GET" || state.method == "DELETE")
    {
        if (!_process_get_delete_request(fd_client, state, request, a, servers, m))
            return;
    }
    else if (state.method == "POST")
    {
        state.timestamp = get_current_timestamp();
        if (!state.complete_metadata)
        {
            if (state.content_type == "multipart/form-data")
            {
                if (!_parse_metadata(state, fd_client, a))
                    return;
            }
            else
            {
                if (state.path == "/login" || state.path == "/check_user" || state.path == "/logout")
                    state.metadata = state.readstring;
                else
                    state.metadata.clear();
                state.complete_metadata = true;
                state.waiting = false;
            }
        }
        state.timestamp = get_current_timestamp();
        if (state.complete_metadata)
        {
            state.timestamp = get_current_timestamp();
            if (!_process_post_request(fd_client, state, a, m, servers[0]))
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
    else
    {
        state.response = ErrorResponse::Error_MethodeNotAllowed(a);
        state.close = true;
        state.cleanup = true;
        state.send_data = true;
    }
}