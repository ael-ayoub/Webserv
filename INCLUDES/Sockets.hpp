#ifndef SOCKETS_HPP
#define SOCKETS_HPP


#include <sys/epoll.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <fcntl.h>      
#include <unistd.h>     
#include <errno.h> 
#include <cstring>
#include <iostream>
#include "Webserv.hpp"

class Config;

void set_nonblocking(int fd);
int CreateSocket(const sockaddr_in &addr);
int CreateEpoll(int fd_socket);
void HandleClient(const int &fd_epoll, const int &fd_client, Config& a);
void  Monitor(const int &fd_socket, const int &fd_epoll, sockaddr_in &addr, Config& a);

#endif