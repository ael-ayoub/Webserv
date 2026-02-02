#include "../INCLUDES/Webserv.hpp"

std::string _getCookies(const std::string &header)
{
    if (header.empty())
        return "";

    size_t pos = header.find("Cookie:");
    if (pos != std::string::npos)
    {
        size_t end = header.find("\n", pos);
        if (end != std::string::npos)
        {
            return header.substr(pos + 7, end - pos - 7);
        }
        else
        {
            return header.substr(pos + 7);
        }
    }

    return "";
}


bool _parse_header(ClientState &state, int fd_client, Request &request, Config &a)
{
    // std::cout << "checking timeout for fd: " << fd_client << std::endl;
    if (!check_timeout(state.timestamp, TIMEOUT))
    {
        std::cout << "Connection timed out for fd: " << fd_client << std::endl;
        state.response = ErrorResponse::Error_BadRequest(a);
        cloce_connection(state);
        return false;
    }
    state.timestamp = get_current_timestamp();
    // std::cout << "Reading header from fd: " << fd_client << std::endl;
    char buffer[4096];
    ssize_t n = read(fd_client, buffer, sizeof(buffer));
    if (n == 0)
    {
        std::cout << "Client disconnected fd: " << fd_client << std::endl;
        state.close = true;
        state.cleanup = true;
        state.send_data = false;
        state.waiting = false;
        return false;
    }
    else if (n < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            std::cout << "No data available to read from fd: " << fd_client << " (EAGAIN/EWOULDBLOCK)" << std::endl;
            return false;
        }
        std::cout << "Error reading from fd: " << fd_client << ", errno: " << errno << std::endl;
        state.close = true;
        state.cleanup = true;
        state.send_data = false;
        return false;
    }
    else
    {
        std::cout << "Read " << n << " bytes from fd: " << fd_client << std::endl;
        state.readstring.append(buffer, n);
        size_t pos = state.readstring.find("\r\n\r\n");
        if (pos != std::string::npos)
        {
            std::cout << "checking timeout for fd: " << fd_client << std::endl;

            if (!check_timeout(state.timestamp, TIMEOUT))
            {
                std::cout << "Connection timed out for fd: " << fd_client << std::endl;
                cloce_connection(state);
                return false;
            }
            state.timestamp = get_current_timestamp();
            std::cout << "Complete header received from fd: " << fd_client << std::endl;
            state.header = state.readstring.substr(0, pos);
            if (state.header.size() > HEADER_SIZE)
            {
                state.response = ErrorResponse::Error_BadRequest(a);
                state.close = true;
                state.cleanup = true;
                state.send_data = true;
                return false;
            }
            state.complete_header = true;
            state.readstring.erase(0, pos + 4);
            request.parse_request(state.header, a);
            state.method = request.get_method();
            state.path = request.get_path();
            state.waiting = false;
            state.content_length = request.get_content_length();
            state.timestamp = get_current_timestamp();
            if (state.header.find("Content-type: multipart/form-data") != std::string::npos)
            {
                size_t boundary_pos = state.header.find("boundary=");
                if (boundary_pos != std::string::npos)
                {
                    state.boundary = "--" + state.header.substr(boundary_pos + 9);
                }
                state.content_type = "multipart/form-data";
            }
            state.cookies = _getCookies(state.header);
            return true;
        }
        else
        {
            std::cout << "checking timeout for fd: " << fd_client << std::endl;
            if (!check_timeout(state.timestamp, TIMEOUT))
            {
                std::cout << "Connection timed out for fd: " << fd_client << std::endl;
                state.close = true;
                state.cleanup = true;
                state.send_data = false;
                state.waiting = false;
                return false;
            }
            state.timestamp = get_current_timestamp();
            std::cout << "Incomplete header, waiting for more data from fd: " << fd_client << std::endl;
            state.waiting = true;
            return false;
        }
        return true;
    }
}

bool _process_get_delete_request(int fd_client, ClientState &state, Request &request, Config &a, std::vector<ServerConfig> &servers, Methodes &m)
{
    try
    {
        std::cout << "############ [..] handle GET/DELETE method for fd: " << fd_client << std::endl;
        if (!check_timeout(state.timestamp, TIMEOUT))
        {
            std::cout << "Connection timed out for fd: " << fd_client << std::endl;
            cloce_connection(state);
            return false;
        }
        state.timestamp = get_current_timestamp();

        state.response = m.GetMethod(a, request, servers[0]);
        state.send_data = true;
        state.cleanup = true;
        if (state.header.find("Connection: close") != std::string::npos)
        {
            std::cout << "the client send Connection: close header, so we close the connection after response.\n";
            state.close = true;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        state.response = ErrorResponse::Error_BadRequest(a);
        state.close = true;
        state.cleanup = true;
        state.send_data = true;
    }
    return true;
}


bool _parse_metadata(ClientState &state, int fd_client, Config &a)
{
    if (!check_timeout(state.timestamp, TIMEOUT))
    {
        std::cout << "Connection timed out for fd: " << fd_client << std::endl;
        state.response = ErrorResponse::Error_BadRequest(a);
        cloce_connection(state);
        return false;
    }
    state.timestamp = get_current_timestamp();
    char buffer[4096];
    ssize_t n = read(fd_client, buffer, sizeof(buffer));
    if (n == 0)
    {
        std::cout << "Client disconnected fd: " << fd_client << std::endl;
        state.close = true;
        state.cleanup = true;
        state.send_data = false;
        state.waiting = false;
        return false;
    }
    else if (n < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            std::cout << "No data available to read from fd: " << fd_client << " (EAGAIN/EWOULDBLOCK)" << std::endl;
            return false;
        }
        std::cout << "Error reading from fd: " << fd_client << ", errno: " << errno << std::endl;
        state.close = true;
        state.cleanup = true;
        state.send_data = false;
        return false;
    }
    else
    {
        std::cout << "Read " << n << " bytes from fd: " << fd_client << std::endl;
        state.readstring.append(buffer, n);
        size_t pos = state.readstring.find("\r\n\r\n");
        if (pos != std::string::npos)
        {
            if (!check_timeout(state.timestamp, TIMEOUT))
            {
                std::cout << "Connection timed out for fd: " << fd_client << std::endl;
                cloce_connection(state);
                return false;
            }
            state.timestamp = get_current_timestamp();
            std::cout << "Complete metadata received from fd: " << fd_client << std::endl;
            state.metadata = state.readstring.substr(0, pos + 4);
            state.readstring.erase(0, pos + 4);
            state.complete_metadata = true;
            state.waiting = false;
            return true;
        }
        else
        {
            if (!check_timeout(state.timestamp, TIMEOUT))
            {
                std::cout << "Connection timed out for fd: " << fd_client << std::endl;
                state.close = true;
                state.cleanup = true;
                state.send_data = false;
                state.waiting = false;
                return false;
            }
            state.timestamp = get_current_timestamp();
            std::cout << "Incomplete metadata, waiting for more data from fd: " << fd_client << std::endl;
            state.waiting = true;
            return false;
        }
        return true;
    }
}


bool _process_post_request(int fd_client, ClientState &state, Request &request, Config &a, std::vector<ServerConfig> &servers, Methodes &m)
{
    (void)request;
    (void)servers;
    try
    {
        std::cout << "############ [..] handle POST method for fd: " << fd_client << std::endl;
        if (!check_timeout(state.timestamp, TIMEOUT))
        {
            std::cout << "Connection timed out for fd: " << fd_client << std::endl;
            cloce_connection(state);
            return false;
        }
        state.timestamp = get_current_timestamp();

        state.response = m.PostMethod(a,fd_client, state);
        state.send_data = true;
        state.cleanup = true;
        if (state.header.find("Connection: close") != std::string::npos)
        {
            std::cout << "the client send Connection: close header, so we close the connection after response.\n";
            state.close = true;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        state.response = ErrorResponse::Error_BadRequest(a);
        state.close = true;
        state.cleanup = true;
        state.send_data = true;
    }
    return true;
}