#include "../INCLUDES/Webserv.hpp"

class ClientInfo
{
public:
    std::string __method;
    std::string __path;
    std::string __content_type;
    int __content_length;
    std::string __boundary;
    std::string __cookie;
    std::string __filename;
    std::string __type_upload;
};

enum state
{
    READ_HEADER,
    READ_METADATA,
    READ_BODY,
    CLOSE,
    KEEP_ALIVE,
    SEND_RESPONSE
};

class ClientProcess
{
public:
    int __fd_client;
    std::string __header;
    std::string __body;
    std::string __metadata;
    std::string __readBuffer;
    std::string __Response;
    ClientInfo __client_info;
    state __state;
    size_t __bytes_sent;
    size_t __fileupload_bytes;

    void _handleClient();
    void _parseHeader();
    void _prepareResponse();
    void _parseMetadata();
    void _generateErrorResponse();
    void _uploadFile(const std::string &data);
};

void ClientProcess::_handleClient()
{
    char buffer[4096];

    while (true)
    {
        ssize_t n = read(__fd_client, buffer, sizeof(buffer));

        if (n > 0)
        {
            __readBuffer.append(buffer, n);
            if (__state == READ_HEADER)
            {
                size_t pos = __readBuffer.find("\r\n\r\n");
                if (pos != std::string::npos)
                {
                    __header = __readBuffer.substr(0, pos + 4);
                    // __body = __readBuffer.substr(pos + 4);
                    __readBuffer.erase(0, pos + 4);
                    _parseHeader();
                    if (__client_info.__content_length > 0)
                    {
                        __state = READ_METADATA;
                        continue;
                    }
                    else
                    {
                        _prepareResponse();
                        __state = SEND_RESPONSE;
                        return;
                    }
                }
            }
            else if (__state == READ_METADATA)
            {
                if (__client_info.__path == "session" || __client_info.__content_type == "multipart/form-data")
                {
                    size_t pos = __readBuffer.find("\r\n\r\n");
                    if (pos != std::string::npos)
                    {
                        __metadata = __readBuffer.substr(0, pos + 4);
                        __readBuffer.erase(0, pos + 4);
                        _parseMetadata();
                        if (__client_info.__content_length == __metadata.size())
                        {
                            _prepareResponse();
                            __state = SEND_RESPONSE;
                            return;
                        }
                        else
                        {
                            __fileupload_bytes = __client_info.__content_length - __metadata.size();
                            __state = READ_BODY;
                            continue;
                        }
                    }
                }
                else
                {
                    __state = READ_BODY;
                    continue;
                }
            }
            else if (__state == READ_BODY)
            {
                if (__fileupload_bytes == 0)
                {
                    _prepareResponse();
                    __state = SEND_RESPONSE;
                    return;
                }
                else
                {
                    _uploadFile(__readBuffer);
                    if (__fileupload_bytes == 0)
                    {
                        _prepareResponse();
                        __state = SEND_RESPONSE;
                        return;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            else if (n == 0)
            {
                __state = CLOSE;
                _generateErrorResponse();
                return;
            }
            else if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            {
                return;
            }
            else
            {
                __state = CLOSE;
                _generateErrorResponse();
                return;
            }
        }
    }
}



void monitor()
{
    cleint state;

    handle client 

    flags: read body continue 

    else 
    {
        send response close or keep alive
        clean 
        if close 
            remove epoll and remove

            if time out close the client
    }
}