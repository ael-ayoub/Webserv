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
// #include "Methodes.hpp"
class Config;
// typedef std::vector<std::pair<std::string, std::string> > mysession;

class ClientState
{
public:
	std::string header;
	std::string readstring;
	std::string metadata;
	std::string filename;
	std::string boundary;
	std::string method;
	std::string path;

	bool close;
	bool cleanup;
	bool send_data;
	bool waiting;

	std::string content_type;
	std::string content_length;
	std::string response;
	std::string cookies;
	unsigned long long timestamp;


	std::pair<std::string, std::string> session;
	int fd_upload;
	bool complete_metadata;	
	int expected_content_length;
	bool complete_header;
	bool complete_upload;
	std::vector<char> tail;

	ClientState()
	{
		complete_metadata = false;
		complete_header = false;
		complete_upload = false;
		expected_content_length = 0;
		close = false;
		cleanup = false;
		waiting = false;	
		send_data = false;
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

void printCurrentTime();
void _sendReaponse(const std::string &response, int fd_client);

#define HEADER_SIZE 1024 * 16
#define TIMEOUT 5000
unsigned long long get_current_timestamp();
bool check_timeout(unsigned long long timestamp, unsigned long long timeout);
void cloce_connection( ClientState& state);


#endif