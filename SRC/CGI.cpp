#include "../includes/CGI.hpp"
#include "../includes/Webserv.hpp"
#include <sstream>
#include <sys/wait.h>

std::string getQueryPath(const std::string &path)
{
    size_t q = path.find('?');
    if (q == std::string::npos)
        return path;
    return path.substr(0, q);
}

void rmCgiFd(int fd_epoll, int cgi_fd, std::map<int, int> &cgi_to_client)
{
    epoll_ctl(fd_epoll, EPOLL_CTL_DEL, cgi_fd, NULL);
    close(cgi_fd);
    cgi_to_client.erase(cgi_fd);
}

bool start_cgi_process(const std::string &cgi_binary, const std::string &script_path, const std::vector<std::string> &envs, int &out_fd, pid_t &pid, const std::string &stdin_file_path)
{
    out_fd = -1;
    pid = -1;
    if (cgi_binary.empty() || script_path.empty())
        return false;

    int pipefd[2];
    if (pipe(pipefd) == -1)
        return false;

    int flags = fcntl(pipefd[0], F_GETFL, 0);
    if (flags == -1 || fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK) == -1)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        return false;
    }

    pid = fork();
    if (pid == -1)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        return false;
    }

    if (pid == 0)
    {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        if (!stdin_file_path.empty())
        {
            int in = open(stdin_file_path.c_str(), O_RDONLY);
            if (in != -1)
            {
                dup2(in, STDIN_FILENO);
                close(in);
            }
        }

        std::vector<char *> argv;
        argv.push_back(const_cast<char *>(cgi_binary.c_str()));
        argv.push_back(const_cast<char *>(script_path.c_str()));
        argv.push_back(NULL);

        std::vector<char *> envp;
        for (size_t i = 0; i < envs.size(); i++)
            envp.push_back(const_cast<char *>(envs[i].c_str()));
        envp.push_back(NULL);

        execve(cgi_binary.c_str(), &argv[0], &envp[0]);
        _exit(1);
    }

    close(pipefd[1]);
    out_fd = pipefd[0];
    return true;
}

std::vector<std::string> build_cgi_env(const std::string &method, const std::string &query_string, size_t content_length, const std::string &content_type, const std::string &script_name)
{
    std::vector<std::string> envs;
    envs.push_back("REQUEST_METHOD=" + method);
    envs.push_back("QUERY_STRING=" + query_string);
    {
        std::ostringstream oss;
        oss << content_length;
        envs.push_back("CONTENT_LENGTH=" + oss.str());
    }
    envs.push_back("CONTENT_TYPE=" + content_type);
    envs.push_back("GATEWAY_INTERFACE=CGI/1.1");
    envs.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envs.push_back("SCRIPT_NAME=" + script_name);
    return envs;
}

bool start_cgi_for_client(ClientState &state, const std::string &cgi_binary, const std::string &script_path, const std::vector<std::string> &envs, const std::string &stdin_file_path)
{
    int out_fd = -1;
    pid_t pid = -1;
    if (!start_cgi_process(cgi_binary, script_path, envs, out_fd, pid, stdin_file_path))
        return false;

    state.cgi_active = true;
    state.cgi_fd = out_fd;
    state.cgi_pid = pid;
    state.cgi_deadline = get_current_timestamp();
    state.cgi_output.clear();
    state.cgi_stdin_path = stdin_file_path;
    state.waiting = true;
    state.send_data = false;
    state.cleanup = false;
    state.close = false;
    return true;
}

bool start_get_cgi_if_needed(ClientState &state, Request &request, Config &config, ServerConfig &server)
{
    if (request.get_method() != "GET")
        return false;

    std::string request_path = request.get_path();
    std::string query_string;
    size_t qpos = request_path.find('?');
    if (qpos != std::string::npos)
    {
        query_string = request_path.substr(qpos + 1);
        request_path = request_path.substr(0, qpos);
    }

    LocationConfig info_location = server.get_conf(getQueryPath(request_path));
    if (info_location.GetRedirectionBool() == true)
        return false;

    std::string fs_path = "." + info_location.get_root() + request_path;
    struct stat statbuf;
    if (stat(fs_path.c_str(), &statbuf) != 0)
        return false;

    std::string script_fs;
    if (S_ISDIR(statbuf.st_mode))
    {
        if (info_location.get_pathIndex() == "None")
            return false;
        script_fs = "." + info_location.get_root() + request_path + info_location.get_pathIndex();
    }
    else
        script_fs = fs_path;

    size_t dot = script_fs.rfind('.');
    if (dot == std::string::npos)
        return false;

    std::string ext = script_fs.substr(dot);
    std::string binary = info_location.get_cgi_binary(ext);
    if (binary.empty())
        return false;

    std::vector<std::string> envs = build_cgi_env(request.get_method(),
                                                  query_string,
                                                  request.get_content_length(),
                                                  "",
                                                  request_path);
    if (!start_cgi_for_client(state, binary, script_fs, envs, ""))
    {
        state.response = ErrorResponse::Error_Internal_Server(config);
        state.send_data = true;
        state.cleanup = true;
        state.waiting = false;
        state.close = true;
    }
    return true;
}

void cleanup_cgi_state(ClientState &state, int fd_epoll, std::map<int, int> &cgi_to_client)
{
    if (state.cgi_fd != -1)
    {
        rmCgiFd(fd_epoll, state.cgi_fd, cgi_to_client);
        state.cgi_fd = -1;
    }
    if (state.cgi_pid > 0)
    {
        kill(state.cgi_pid, SIGKILL);
        waitpid(state.cgi_pid, NULL, 0);
        state.cgi_pid = -1;
    }
    if (!state.cgi_stdin_path.empty())
    {
        std::remove(state.cgi_stdin_path.c_str());
        state.cgi_stdin_path.clear();
    }
    state.cgi_output.clear();
    state.cgi_active = false;
    state.cgi_deadline = 0;
}

void finalize_cgi_success(ClientState &state, int fd_epoll, std::map<int, int> &cgi_to_client)
{
    if (state.cgi_fd != -1)
    {
        rmCgiFd(fd_epoll, state.cgi_fd, cgi_to_client);
        state.cgi_fd = -1;
    }
    if (state.cgi_pid > 0)
    {
        waitpid(state.cgi_pid, NULL, WNOHANG);
        state.cgi_pid = -1;
    }
    if (!state.cgi_stdin_path.empty())
    {
        std::remove(state.cgi_stdin_path.c_str());
        state.cgi_stdin_path.clear();
    }

    state.response = build_cgi_http_response(state.cgi_output);
    state.cgi_output.clear();
    state.cgi_active = false;
    state.cgi_deadline = 0;
    state.waiting = false;
    state.send_data = true;
    state.cleanup = true;
}

std::string build_cgi_http_response(const std::string &raw_output)
{
    std::string statusLine = "HTTP/1.1 200 OK\r\n";
    std::string normalized;
    normalized.reserve(raw_output.size() + 16);

    for (size_t i = 0; i < raw_output.size(); i++)
    {
        if (raw_output[i] == '\n')
        {
            if (i == 0 || raw_output[i - 1] != '\r')
                normalized += "\r\n";
            else
                normalized += "\n";
        }
        else
            normalized += raw_output[i];
    }

    if (normalized.find("Content-Type:") != normalized.npos)
    {
        size_t sep = normalized.find("\r\n\r\n");
        if (sep == std::string::npos)
        {
            std::ostringstream contentLengthStream;
            contentLengthStream << normalized.length();
            return statusLine +
                "Content-Type: text/html\r\n"
                "Content-Length: " + contentLengthStream.str() + "\r\n"
                "Connection: close\r\n\r\n" +
                normalized;
        }

        std::string cgi_headers = normalized.substr(0, sep);
        std::string cgi_body = normalized.substr(sep + 4);
        bool has_content_length = (cgi_headers.find("Content-Length:") != std::string::npos);
        bool has_connection = (cgi_headers.find("Connection:") != std::string::npos);

        std::string extra;
        if (!has_content_length)
        {
            std::ostringstream contentLengthStream;
            contentLengthStream << cgi_body.length();
            extra += "Content-Length: " + contentLengthStream.str() + "\r\n";
        }
        if (!has_connection)
            extra += "Connection: close\r\n";

        return statusLine + cgi_headers + "\r\n" + extra + "\r\n" + cgi_body;
    }

    std::ostringstream contentLengthStream;
    contentLengthStream << normalized.length();
    std::string header =
        "Content-Type: text/html\r\n"
        "Content-Length: " + contentLengthStream.str() + "\r\n\r\n";

    return statusLine + header + normalized;
}
