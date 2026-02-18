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

#define HEADER_SIZE 1024 * 16
#define TIMEOUT 5000

class ClientState
{
public:
	std::string header;
	std::string readstring;
	std::string metadata;
	std::string raw_content_type;
	std::string filename;
	std::string boundary;
	std::string end_boundary;
	std::string method;
	std::string path;
	std::string filename_upload;
	size_t byte_uploaded;
	size_t body_received;

	bool close;
	bool cleanup;
	bool send_data;
	bool waiting;

	std::string content_type;
	size_t content_length;
	std::string response;
	std::string cookies;
	unsigned long long timestamp;

	std::pair<std::string, std::string> session;
	int fd_upload;
	int fd_body;
	std::string body_tmp_path;
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
		fd_body = -1;
		timestamp = 0;
		byte_uploaded = 0;
		body_received = 0;
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

// void _setCookies(std::string &username);

void printCurrentTime();
void _sendReaponse(const std::string &response, int fd_client);

unsigned long long get_current_timestamp();
bool check_timeout(unsigned long long timestamp, unsigned long long timeout);
void cloce_connection(ClientState &state);

#endif