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
		for (size_t i = 0; i < tmp_a.size(); i++)
			ports.push_back(tmp_a[i].get_ip());

		Socket socket(ports);
		socket.run(a);
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << "\n";
	}
}