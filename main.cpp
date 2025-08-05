#include "INCLUDES/Webserv.hpp"

int main()
{
    Config a;
    if (a.store_file("/home/abahaded/Desktop/Webserv/SRC/default.conf") == -1)
        std::cerr << "Error in the Config file" << std::endl;
    a.print_confiFile();
    // a.Servers_Config[0].print_info_server();
    // ServerConfig server;
    // server.parse_config(a.get_file_lines(), );
}

// store every object in class ServerConfig
    // get index from to from the config class 