#include "../includes/Webserv.hpp"
#include "../includes/CGI.hpp"

static std::string _lower(const std::string &s)
{
    std::string out = s;
    for (size_t i = 0; i < out.size(); i++)
        out[i] = (char)std::tolower(out[i]);
    return out;
}

static size_t content_length(const std::string &header)
{
    std::string h = _lower(header);
    const std::string key = "content-length:";
    size_t pos = h.find(key);
    if (pos == std::string::npos)
        return 0;
    pos += key.size();
    while (pos < h.size() && (h[pos] == ' ' || h[pos] == '\t'))
        pos++;
    size_t end = h.find("\r\n", pos);
    if (end == std::string::npos)
        end = h.size();
    std::string len_str = header.substr(pos, end - pos);
    std::stringstream ss(len_str);
    size_t length = 0;
    ss >> length;
    return length;
}

static std::string _header_value(const std::string &header, const std::string &key)
{
    std::string h = _lower(header);
    std::string needle = _lower(key) + ":";
    size_t pos = h.find(needle);
    if (pos == std::string::npos)
        return "";
    pos += needle.size();
    while (pos < header.size() && (header[pos] == ' ' || header[pos] == '\t'))
        pos++;
    size_t end = header.find("\r\n", pos);
    if (end == std::string::npos)
        end = header.size();
    return header.substr(pos, end - pos);
}

static std::string _boundary_from_content_type(const std::string &content_type)
{
    std::string ct = _lower(content_type);
    size_t b = ct.find("boundary=");
    if (b == std::string::npos)
        return "";
    b += 9;
    size_t end = content_type.find(';', b);
    if (end == std::string::npos)
        end = content_type.find(' ', b);
    if (end == std::string::npos)
        end = content_type.size();
    std::string val = content_type.substr(b, end - b);
    if (val.size() >= 2 && val[0] == '"' && val[val.size() - 1] == '"')
        val = val.substr(1, val.size() - 2);
    return val;
}

bool _parse_header(ClientState &state, int fd_client, Request &request, Config &a)
{
    if (!times_out(state.timestamp, TIMEOUT))
    {
        state.response = ErrorResponse::Error_BadRequest(a);
        cloce_connection(state);
        return false;
    }
    state.timestamp = get_current_timestamp();
    char buffer[4096];
    ssize_t n = read(fd_client, buffer, sizeof(buffer));
    if (n == 0)
    {
        state.close = true;
        state.cleanup = true;
        state.send_data = false;
        state.waiting = false;
        return false;
    }
    else if (n < 0)
    {
        state.close = true;
        state.cleanup = true;
        state.send_data = false;
        state.waiting = false;
        return false;
    }
    else
    {
        buffer[n] = '\0'; 
        state.readstring.append(buffer, n);
        size_t pos = state.readstring.find("\r\n\r\n");
        if (pos != std::string::npos)
        {
            if (!times_out(state.timestamp, TIMEOUT))
            {
                cloce_connection(state);
                return false;
            }
            state.timestamp = get_current_timestamp();
            state.header = state.readstring.substr(0, pos + 2);  // Include the final \r\n
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
            std::string parse_res = request.parse_request(state.header, a);
            if (parse_res != "NONE")
            {
                state.response = parse_res;
                state.close = true;
                state.cleanup = true;
                state.send_data = true;
                state.waiting = false;
                return false;
            }

            state.method = request.get_method();
            state.path = request.get_path();
            state.content_length = content_length(state.header);
            state.timestamp = get_current_timestamp();

            state.raw_content_type = _header_value(state.header, "Content-Type");
            if (_lower(state.raw_content_type).find("multipart/form-data") != std::string::npos)
            {
                std::string b = _boundary_from_content_type(state.raw_content_type);
                if (!b.empty())
                    state.boundary = "--" + b;
                if (!state.boundary.empty())
                    state.end_boundary = state.boundary + "--";
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
            if (!times_out(state.timestamp, TIMEOUT))
            {
                state.close = true;
                state.cleanup = true;
                state.send_data = false;
                state.waiting = false;
                return false;
            }
            state.timestamp = get_current_timestamp();
            state.waiting = true;
            return false;
        }
        return true;
    }
}

bool _process_get_delete_request(int fd_client, ClientState &state, Request &request, Config &a, std::vector<ServerConfig> &servers, Methodes &m)
{
    (void) fd_client;
    try
    {
        if (!times_out(state.timestamp, TIMEOUT))
        {
            cloce_connection(state);
            return false;
        }
        state.timestamp = get_current_timestamp();

        if (!state.cgi_active && processCGI(state, request, a, servers[0]))
            return true;
        if (state.cgi_active)
        {
            state.waiting = true;
            return true;
        }

        
        state.response = m.GetMethod(a, request, servers[0]);
        state.send_data = true;
        state.cleanup = true;
        if (state.header.find("Connection: close") != std::string::npos)
        {
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
    if (!times_out(state.timestamp, TIMEOUT))
    {
        state.response = ErrorResponse::Error_BadRequest(a);
        cloce_connection(state);
        return false;
    }
    state.timestamp = get_current_timestamp();
    char buffer[4096];
    size_t pos = state.readstring.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        state.metadata = state.readstring.substr(0, pos + 4);
        state.readstring.erase(0, pos + 4);
        state.complete_metadata = true;
        state.waiting = false;
        return true;
    }
    ssize_t n = read(fd_client, buffer, sizeof(buffer));
    if (n == 0)
    {
        state.close = true;
        state.cleanup = true;
        state.send_data = false;
        state.waiting = false;
        return false;
    }
    else if (n < 0)
    {
        state.waiting = true;
        return false;
    }
    else
    {
        state.readstring.append(buffer, n);
        size_t pos = state.readstring.find("\r\n\r\n");
        if (pos != std::string::npos)
        {
            if (!times_out(state.timestamp, TIMEOUT))
            {
                // std::cout << "Connection timed out for fd: " << fd_client << std::endl;
                cloce_connection(state);
                return false;
            }
            state.timestamp = get_current_timestamp();
            // std::cout << "Complete metadata received from fd: " << fd_client << std::endl;
            state.metadata = state.readstring.substr(0, pos + 4);
            state.readstring.erase(0, pos + 4);
            state.complete_metadata = true;
            state.waiting = false;
            return true;
        }
        else
        {
            if (!times_out(state.timestamp, TIMEOUT))
            {
                // std::cout << "Connection timed out for fd: " << fd_client << std::endl;
                state.close = true;
                state.cleanup = true;
                state.send_data = false;
                state.waiting = false;
                return false;
            }
            state.timestamp = get_current_timestamp();
            // std::cout << "Incomplete metadata, waiting for more data from fd: " << fd_client << std::endl;
            state.waiting = true;
            return false;
        }
        return true;
    }
}


bool _process_post_request(int fd_client, ClientState &state, Config &a, Methodes &m, ServerConfig& selected_server)
{
        if(!times_out(state.timestamp, TIMEOUT))
        {
            cloce_connection(state);
            return false;
        }
        state.timestamp = get_current_timestamp();

        m.PostMethod(a, fd_client, state, selected_server);
        if (state.waiting)
            return true;

        if (!state.send_data)
        {
            state.response = ErrorResponse::Error_BadRequest(a);
            state.send_data = true;
            state.cleanup = true;
            state.close = true;
        }

        if (state.header.find("Connection: close") != std::string::npos)
            state.close = true;
    return true;
}