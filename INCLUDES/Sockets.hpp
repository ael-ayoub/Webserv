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
#include "socketConfig.hpp"

class Config;
typedef std::vector<std::pair<std::string, std::string> > mysession;

class ClientState
{
public:
	std::string header;
	std::string metadata;
	std::string filename;
	std::string boundary;
	std::string method;
	std::string path;
	std::pair<std::string, std::string> session;
	int fd_upload;
	bool complete_metadata;
	bool complete_header;
	bool complete_upload;
	std::vector<char> tail;

	ClientState()
	{
		complete_metadata = false;
		complete_header = false;
		complete_upload = false;
		// data_send = false;
		fd_upload = -1;
	}
};

class Socket
{
private:
	Methodes m;
	std::vector<SockConf> sockconf;
	int fd_epoll;

public:
	Socket(std::vector<std::pair<std::string, int> > ports);
	void run(Config &a);
	void set_nonblocking(int fd);
	void CreateSocket();
	void CreateEpoll();
	int checkEvent(int fd);
	void HandleClient(int fd_client, Config &a, std::map<int, ClientState> &status);
	void Monitor(Config &a);
};

std::string generateSuccessMsg();
std::string generateFailerMsg();
bool _uploadFile(const int &fd_client, ClientState &state);

bool _setSession(std::string &response);
bool _getSession(std::string &response);
bool _removeName(std::string &responce);
void _setCookies(ClientState& state);
std::vector<std::pair<std::string, std::string> > _loadData();

#endif