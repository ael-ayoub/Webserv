#include "../../INCLUDES/CGI.hpp"
#include "../../INCLUDES/Upload.hpp"
#include "../../INCLUDES/includes.hpp"

int main(int ac, char **av)
{
	const char *env[] = {
		"REQUEST_METHOD=GET",
		"QUERY_STRING=name=John&age=30",
		"CONTENT_LENGTH=0",
		"CONTENT_TYPE=",
		"GATEWAY_INTERFACE=CGI/1.1",
		"SCRIPT_NAME=/cgi-bin/hello.cgi",
		"SERVER_NAME=localhost",
		"SERVER_PORT=8080",
		"SERVER_PROTOCOL=HTTP/1.1",
		"SERVER_SOFTWARE=MyWebServer/1.0",
		"REMOTE_ADDR=127.0.0.1",
		"REMOTE_PORT=12345",
		NULL
	};

	std::ifstream image1("/home/asoudani/img.jpeg");
	std::stringstream image;
	image << image1.rdbuf();

	std::string request =
	    "POST /upload HTTP/1.1\r\n"
		"Host: localhost:8080\r\n"
		"Content-Type: image/png\r\n"
		"Content-Length: 20\r\n"
		"\r\n"
		"\x89PNG\r\n\x1A\n....binarydata....";
	

	    // "POST /upload HTTP/1.1\r\n"
		// "Host: localhost:8080\r\n"
		// "Content-Type: application/octet-stream\r\n"
		// "Content-Disposition: attachment; filename=\"notes.txt\"\r\n"
		// "Content-Length: 32\r\n"
		// "\r\n"
		// "Hello, this is the file content!";


		// "POST /upload HTTP/1.1\r\n"
		// "Host: localhost\r\n"
		// "User-Agent: CustomUploader/1.0\r\n"
		// "Accept: */*\r\n"
		// "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryA1B2C3D4E5\r\n"
		// "Content-Length: 612\r\n\r\n"
		// "------WebKitFormBoundaryA1B2C3D4E5\r\n"
		// "Content-Disposition: form-data; name=\"username\"\r\n\r\n"
		// "Ayoub_S\r\n"
		// "------WebKitFormBoundaryA1B2C3D4E5\r\n"
		// "Content-Disposition: form-data; name=\"myfile\"; filename=\"hello.txt\"\r\n"
		// "Content-Type: text/plain\r\n\r\n"
		// "Hello world from my file!\r\n"
		// "This file has multiple lines of text.\r\n"
		// "Line 1: Example data\r\n"
		// "Line 2: Another example\r\n"
		// "Line 3: Final line in text file.\r\n\r\n"
		// "------WebKitFormBoundaryA1B2C3D4E5\r\n"
		// "Content-Disposition: form-data; name=\"image\"; filename=\"picture.jpg\"\r\n"
		// "Content-Type: image/jpeg\r\n\r\n"
		// // + image.str() +
		// "\r\n"
		// "------WebKitFormBoundaryA1B2C3D4E5--\r\n";

	// CGI out("GET", "./hello.py", ".py", "/usr/bin/python3", env);

	// const char *cgi_binary = "/usr/bin/python3";
	// const char *file = "./hello.py";
	// const char **arguments = new const char *[3];
	// arguments[0] = cgi_binary;
	// arguments[1] = file;
	// arguments[2] = NULL;
	
    // if (out.CGIProccess() == false)
    //     return 1;
	// std::cout << out.response;

	Upload out(request);
	out.printChunks();
    return 0;
}

// ! TODO : make sure to handle content type image.. use std::vector<char> to store it.

// todo: convert a char * to char ** (split request buffre[1024])