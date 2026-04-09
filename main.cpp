#include "INCLUDES/Webserv.hpp"

int main(int ac, char **av)
{
    if (ac == 2)
        run_server(av[1]);
    else
        run_server("");
}
