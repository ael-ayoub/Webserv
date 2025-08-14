#include "../INCLUDES/Webserv.hpp"

void run_server()
{
	try
	{
		Config a;
		std::vector<std::pair<std::string, int> > ports;

		a.store_file("/home/ael-aiss/Desktop/Webserv/SRC/default.conf");
		std::vector<ServerConfig> tmp_a = a.get_allserver_config();

		for (size_t i = 0; i < tmp_a.size(); i++)
			ports.push_back(tmp_a[i].get_ip());
		Socket socket(ports);
		socket.run(a);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}