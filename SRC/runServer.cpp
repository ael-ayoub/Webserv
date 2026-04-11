#include "../includes/Webserv.hpp"

std::string get_current_path()
{
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
        return std::string(buffer);
    return "";
}

std::vector<std::pair<std::string, int> > filterServerPorts(std::vector<ServerConfig> servers)
{
	std::vector<std::pair<std::string, int> > store_ports;
	size_t o = 0;
	while (o < servers.size())
	{
		std::vector<std::pair<std::string, int> > port_server = servers[o].get_ip();
		size_t j = 0;
		while (j < port_server.size())
		{
			store_ports.push_back(port_server[j]);
			j++;
		}
		o++;
	}
	o = 0;
	while (o < store_ports.size())
	{
		size_t j = o + 1;
		while (j < store_ports.size())
		{
			if (store_ports[o].second == store_ports[j].second)
				store_ports.erase(store_ports.begin() + j);
			else
				j++;
		}
		o++;
	}

	return store_ports;
}

void run_server(std::string av)
{
	try
	{
		Config a;

		a.store_file(av);
		Socket socket(filterServerPorts(a.get_allserver_config()));
		socket.run(a);
	}
	catch (std::exception &e) 
	{
		std::cout << e.what() << std::endl;
	}
}