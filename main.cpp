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
        a.store_file("/home/ael-aiss/Desktop/Webserv/SRC/default.conf");
    }
    catch(const Config::ErrorSyntax& e)
    {
        std::cerr << e.what() << '\n';
    }

    if (!socket.run())
    {
        std::cout << "cannod run the rerver !!!" << std::endl;
    }
}
