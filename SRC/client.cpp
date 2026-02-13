#include "../INCLUDES/Webserv.hpp"


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


// std::string CheckSession(const std::string &message)
// {
//     std::string body = message;

//     std::string response;
//     response = "HTTP/1.1 200 OK\r\n";
//     response += "Content-Type: text/plain\r\n";
//     std::stringstream ss;
//     ss << body.size();
//     response += "Content-Length: " + ss.str() + "\r\n";
//     if ()
//     response += "\r\n";
//     response += body;

//     return response;
// }

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

bool check_timeout(unsigned long long timestamp, unsigned long long timeout)
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
    std::cout << "Marked connection for closure." << std::endl;
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
    state.timestamp = get_current_timestamp();
    if (state.method == "GET" || state.method == "DELETE")
    {
        if (!_process_get_delete_request(fd_client, state, request, a, servers, m))
            return;
    }

    else if (state.method == "POST")
    {
        // std::cout << "client body size is : " << ServerConfig::client_max_body_size << std::endl;
        state.timestamp = get_current_timestamp();
        if (!state.complete_metadata)
        {
            // std::cout << "############ [..] reading metadata for fd: " << fd_client << std::endl;
            if (state.content_type == "multipart/form-data")
            {
                if (!_parse_metadata(state, fd_client, a))
                    return;
            }
            else
            {
                // std::cout << "im hare "
                // std::cout << "############ [..] handle normal metadata for fd: " << fd_client << std::endl;
                state.metadata = state.readstring;
                state.complete_metadata = true;
                state.waiting = false;
            }
        }
        state.timestamp = get_current_timestamp();
        if (state.complete_metadata)
        {
            state.timestamp = get_current_timestamp();
            if (!_process_post_request(fd_client, state, a, m))
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