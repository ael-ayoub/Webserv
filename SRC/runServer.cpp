#include "../INCLUDES/Webserv.hpp"

std::string get_current_path()
{
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
        return std::string(buffer);
    return "";
}

void run_server()
{
	try
	{
		Config a;
		std::vector<std::pair<std::string, int> > ports;
		std::string path = get_current_path() + "/SRC/default.conf";
		a.store_file(path);

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