#include "../includes/Webserv.hpp"

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
		// std::vector<std::pair<std::string, int> > ports; //comment it 
		std::vector<std::vector<std::pair<std::string, int> > > ports;
		Config a;

		a.store_file(av);

		std::vector<ServerConfig> tmp_a = a.get_allserver_config();

		size_t i = 0;
		while (tmp_a.size() > i)
		{
			ports.push_back(tmp_a[i].get_ip());
			// ports = tmp_a[0].get_ip(); //comment it
			i++;
		}

		Socket socket(ports); // change the prototype
		socket.run(a);
	}
	catch (...) {}
}