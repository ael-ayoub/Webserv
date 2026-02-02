#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <limits.h>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <bits/stdc++.h>
// #include "ClientState.hpp"

typedef std::vector<std::string> Vector_str;
void run_server(std::string av);
#include "ServerConfig.hpp"
#include "Config.hpp"
#include "LocationConfig.hpp"
#include "Syntax_server.hpp"
#include "Syntax_location.hpp"
#include "Methodes.hpp"
#include "Sockets.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ErrorResponse.hpp"
#include "socketConfig.hpp"
#include <sys/time.h>

bool _parse_header(ClientState &state, int fd_client, Request &request, Config &a);
bool _process_get_delete_request(int fd_client, ClientState &state, Request &request, Config &a, std::vector<ServerConfig> &servers, Methodes &m);
bool _parse_metadata(ClientState &state, int fd_client, Config &a);
bool _process_post_request(int fd_client, ClientState &state, Request &request, Config &a, std::vector<ServerConfig> &servers, Methodes &m);