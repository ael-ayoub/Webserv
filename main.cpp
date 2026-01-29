#include "INCLUDES/Webserv.hpp"

// std::string DefaultConfigFile(Config &a)
// {
//     std::string path = "/tmp/default.conf";
//     std::ofstream File(path.c_str());

//     if (!File.is_open())
//     {
//         std::cerr << "Error when creating default config file!\n";
//         return "";
//     }

    
//     File.close();
//     return path;
// }

int main(int ac, char **av)
{
    if (ac == 2)
        run_server(av[1]);
    else
        run_server("");
}
