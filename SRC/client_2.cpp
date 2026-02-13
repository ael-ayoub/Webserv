#include "../INCLUDES/Webserv.hpp"

// std::string _getCookies(const std::string &header)
// {
//     std::string username_key = "username=";
// 	size_t pos = header.find(username_key);
// 	if (pos != std::string::npos)
// 	{
// 		pos += username_key.length();
// 		size_t end_pos = header.find("\r\n", pos);
// 		if (end_pos != std::string::npos)
// 		{
// 			return header.substr(pos, end_pos - pos);
// 		}
// 		else
// 		{
// 			return header.substr(pos);
// 		}
// 	}
// 	return "";
// }

size_t content_length(const std::string &header)
{
    size_t pos = header.find("Content-Length:");
    if (pos != std::string::npos)
    {
        size_t end = header.find("\r\n", pos);
        if (end != std::string::npos)
        {
            std::string len_str = header.substr(pos + 15, end - pos - 15);
            // return static_cast<size_t>(std::stoul(len_str));
            std::stringstream ss(len_str);
            size_t length;
            ss >> length;
            return length;
        }
    }
    return 0;
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
            int len =  request.get_content_length();
            std::cout << "content_length: " << len << std::endl;
            state.method = request.get_method();
            state.path = request.get_path();
            state.content_length = content_length(state.header);
            // std::cout << "Content-Length: " << state.content_length << std::endl;
            state.timestamp = get_current_timestamp();
            if (state.header.find("Content-Type: multipart/form-data") != std::string::npos)
            {
                size_t boundary_pos = state.header.find("boundary=");
                if (boundary_pos != std::string::npos)
                {
                    state.boundary = "--" + state.header.substr(boundary_pos + 9, state.header.find("\r\n", boundary_pos) - (boundary_pos + 9));
                    state.end_boundary = state.boundary + "--";
                }
                state.content_type = "multipart/form-data";
            }
            else 
            {
                state.content_type = "other";
            }

            state.cookies = get_username_from_metadata(state.header);
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
        // std::cout << "############ [..] handle GET/DELETE method for fd: " << fd_client << std::endl;
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
    // check if metatdata in readstring
    size_t pos = state.readstring.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        std::cout << "Complete metadata received from fd: " << fd_client << std::endl;
        state.metadata = state.readstring.substr(0, pos + 4);
        state.readstring.erase(0, pos + 4);
        state.complete_metadata = true;
        state.waiting = false;
        return true;
    }
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
            state.waiting = true;
            state.send_data = false;
            state.cleanup = false;
            state.close = false;
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


bool _process_post_request(int fd_client, ClientState &state, Config &a, Methodes &m)
{
        if (!check_timeout(state.timestamp, TIMEOUT))
        {
            std::cout << "Connection timed out for fd: " << fd_client << std::endl;
            cloce_connection(state);
            return false;
        }
        state.timestamp = get_current_timestamp();

        m.PostMethod(a,fd_client, state);
        state.send_data = true;
        state.cleanup = true;
        if (state.header.find("Connection: close") != std::string::npos)
        {
            std::cout << "the client send Connection: close header, so we close the connection after response.\n";
            state.close = true;
        }
    // }
    return true;
}