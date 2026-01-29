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