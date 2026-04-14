#ifndef CGI_HPP
#define CGI_HPP

#include "includes.hpp"

class ClientState;
class Request;
class Config;
class ServerConfig;

bool start_cgi_process(const std::string &cgi_binary, const std::string &script_path, const std::vector<std::string> &envs, int &out_fd, pid_t &pid, const std::string &stdin_file_path);

std::vector<std::string> build_cgi_env(const std::string &method, const std::string &query_string, size_t content_length, const std::string &content_type, const std::string &script_name);

bool startCGI(ClientState &state, const std::string &cgi_binary, const std::string &script_path, const std::vector<std::string> &envs, const std::string &stdin_file_path);

bool processCGI(ClientState &state, Request &request, Config &config, ServerConfig &server);

void cleanup_cgi_state(ClientState &state, int fd_epoll, std::map<int, int> &cgi_to_client);
void finalize_cgi_success(ClientState &state, int fd_epoll, std::map<int, int> &cgi_to_client);

std::string build_cgi_http_response(const std::string &raw_output);

#endif