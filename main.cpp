#include "INCLUDES/Webserv.hpp"

#include <bits/stdc++.h>

void printVector(std::vector<SockConf> vect)
{
    for (size_t i = 0; i < vect.size(); i++)
    {
        std::cout << vect[i].fd_socket << std::endl;
    }
}

std::vector<int> getPorts(std::vector<ServerConfig> confg)
{
    size_t i = 0;
    std::vector<int> ports;
    while (i < confg.size())
    {
        std::pair<std::string, int> tmp = confg[i].get_ip();
        int ip = tmp.second;
        ports.push_back(ip);
    }
    // sort(ports.begin(), ports.end());
    // std::vector<int> iterator = unique(ports.begin(), ports.end());
    // printVector(ports);
    return ports;
}

int main()
{
    // here  i will add new constractor that take same param from config file
    // for replace tmp value of the socket ;
    // socket(parameters form config file);
    try
    {
        Config a;
        a.store_file("/home/ael-aiss/Desktop/Webserv/SRC/default.conf");
        std::vector<ServerConfig> tmp_a = a.get_allserver_config();
        // size_t i = 0;
        // while (i < tmp_a.size())
        //{
        //     std::pair<std::string, int> ip = tmp_a[i].get_ip();
        //     std::cout << "add :" << ip.first << " ip: " << ip.second << std::endl;
        //     i++;
        // }
        std::vector<int> ports;
        ports.push_back(8080);
        ports.push_back(8082);
        ports.push_back(8084);

        Socket socket(ports);
        if (!socket.run(a))
        {
            std::cout << "cannod run the rerver !!!" << std::endl;
        }
    }
    catch (const Config::ErrorSyntax &e)
    {
        std::cerr << e.what() << '\n';
    }
}

// what should do ??? the but is:

// to assigen addr to every socket , and add fd_socket to every
