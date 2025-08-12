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
