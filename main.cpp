#include "INCLUDES/Webserv.hpp"

int main()
{
    // here  i will add new constractor that take same param from config file 
    // for replace tmp value of the socket ;
    // socket(parameters form config file);
    Socket socket;
    try
    {
        Config a;
        a.store_file("/home/abahaded/Desktop/Webserv/SRC/default.conf");
        // std::vector<ServerConfig> tmp_a = a.get_allserver_config();
        // size_t i = 0;
        // while (i < tmp_a.size())
        // {
        //     std::pair<std::string, int> ip = tmp_a[i].get_ip();
        //     std::cout << "add :" << ip.first << " ip: " << ip.second << std::endl;
        //     i++;
        // }
        if (!socket.run(a))
        {
            std::cout << "cannod run the rerver !!!" << std::endl;
        }
    }
    catch(const Config::ErrorSyntax& e)
    {
        std::cerr << e.what() << '\n';
    }

}
