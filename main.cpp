#include "INCLUDES/Webserv.hpp"

int main()
{
    Config a;
    a.store_file("/home/abahaded/Desktop/Webserv/SRC/default.conf");
    // a.Servers_Config[0].print_info_server();
    // ServerConfig server;
    // server.parse_config(a.get_file_lines(), );
}

// store every object in class ServerConfig
    // get index from to from the config class 

// this is what u need : 

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int main() {
	int fd;
	sockaddr_in addr;

	std::string html = R"(<!DOCTYPE html>
<html>
<head>
  <title>مرحبا</title>
</head>
<body>
  <h1>مرحبا بك في سيرفري!</h1>
  <a href="/next">اذهب إلى الصفحة التالية</a>
</body>
</html>
)";

	std::string response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/html; charset=UTF-8\r\n";
	response += "Content-Length: " + std::to_string(html.size()) + "\r\n";
	response += "\r\n";
	response += html;

	// Create socket
	fd = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = INADDR_ANY;

	// Bind and listen
	bind(fd, (sockaddr *)&addr, sizeof(addr));
	listen(fd, 5);

	socklen_t sizeaddr = sizeof(addr);

	while (1) {
		int new_fd = accept(fd, (sockaddr *)&addr, &sizeaddr);
		int fdp = fork();

		if (fdp == 0) {  // In child
			close(fd);  // child doesn't need the listening socket

			char buff[100];
			ssize_t bytes;

			while ((bytes = read(new_fd, buff, sizeof(buff) - 1)) > 0) {
				buff[bytes] = '\0';
				printf("%s", buff);
				if (strstr(buff, "\r\n\r\n")) break;  // end of HTTP headers
			}

			send(new_fd, response.c_str(), response.size(), 0);
			close(new_fd);
			exit(0);  // child exits
		}
		else {
			close(new_fd);  // parent closes its copy
		}
	}
}

