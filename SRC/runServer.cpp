#include "../INCLUDES/Webserv.hpp"

std::string get_current_path()
{
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
        return std::string(buffer);
    return "";
}

void run_server(std::string av)
{
	try
	{
		Config a;
		std::vector<std::pair<std::string, int> > ports;
		// std::cout << "HERE\n";
		a.store_file(av);
		// std::cout << "HEEEERE\n";

		std::vector<ServerConfig> tmp_a = a.get_allserver_config();

		ports = tmp_a[0].get_ip();
		// std::cout << ports[0].second << std::endl;
		// std::cout << ports[1].second << std::endl;

		Socket socket(ports);
		socket.run(a);
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << "\n";
	}
}