#include "../../INCLUDES/Webserv.hpp"
#include <dirent.h>  

std::string Methodes::GetMethod(Config a, char *buffer)
{
    (void)a;
    std::string s;
    std::string body, line;
    Request test_request;
    test_request.parse_request(buffer);
    std::cout << "Get request of " << test_request.get_path() << std::endl;
    if (test_request.get_path() == "/favicon.ico")
        return std::string(); ///////////////////// check heree the favicon.co why he display everytume
    LocationConfig info_location = a.get_info_location(test_request.get_path());
    std::cout << "we will work with the location " << info_location.get_path() << std::endl;
    std::string path = info_location.get_root() + test_request.get_path();
    struct stat statbuf;
	std::string statusLine = "HTTP/1.1 200 OK\r\n";

    if (stat(path.c_str(), &statbuf) == 0)
    {
        if (S_ISDIR(statbuf.st_mode))
        {
            if (info_location.get_pathIndex() == "None")
            {
                if (info_location.get_autoIndex() == false && info_location.get_pathIndex() == "None")
                    std::cout << "autoindex is off and index not found\n";
                else
                {
                    DIR* dir = opendir(path.c_str());
                    if (!dir)
                        std::cerr << "Error dir\n";
                    body += "<html>\n";
                    body += "<head><title>Index of ";
                    body += info_location.get_path();
                    body += "</title></head>\n";
                    body += "<link rel=\"icon\" href=\"/favicon.ico\">";
                    body += "<body>\n";
                    body += "<h1>Index of /images/</h1>\n";
                    body += "<ul>\n";
                    struct dirent* entry;
                    while ((entry = readdir(dir)))
                    {
                        if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
                        {
                            body += "    <li><a href=\"" + std::string(entry->d_name) + "\">";
                            body += std::string(entry->d_name) + "</a></li>\n";
                        }
                    }
                    body += "    <li><a href=\"None\">";
                    body += "None</a></li>\n";
                    body += "</ul>\n";
                    body += "</body>\n";
                    body += "</html>\n";
                    closedir(dir);
                    std::ostringstream oss;
                    oss << body.size();
                    s = oss.str();
                }
            }
            else
            {
                std::ifstream file((info_location.get_root() + "/" + info_location.get_pathIndex()).c_str() , std::ios::in);
	            if (!file.is_open())
	            {
	            	return "";
	            	// here should send error message or page depend on our implimentation;
	            }
	            while (getline(file, line))
	            	body += line + "\n";
                
	            std::stringstream ss ;
	            ss << body.size();
	            s = ss.str();
            }
        }
        else
        {
            std::ifstream file((info_location.get_root() + "/" + info_location.get_pathIndex()).c_str() , std::ios::in);
	        if (!file.is_open())
	        {
	        	return "";
	        	// here should send error message or page depend on our implimentation;
	        }
	        while (getline(file, line))
	        	body += line + "\n";
            
	        std::stringstream ss ;
	        ss << body.size();
	        s = ss.str();
        }
    }
    else
    {
        // std::cerr << "Not found\n";
        ServerConfig tmp = a.get_server_config();
        std::vector<std::map<int, std::string> > error = tmp.get_error_status();

        std::map<int, std::string>::iterator err_tmp = error[0].begin();
        // while (err_tmp != error[0].end())
        // {
            // std::cout << "error status " << err_tmp->first << std::endl;
            // std::cout << "error path " << err_tmp->second << std::endl;
        //     err_tmp++;
        // }
	    statusLine = "HTTP/1.1 ";
        statusLine += err_tmp->first;
        statusLine += " OK\r\n";
        std::ifstream file((err_tmp->second).c_str() , std::ios::in);
	    if (!file.is_open())
	    {
	    	return "";
	    	// here should send error message or page depend on our implimentation;
	    }
	    while (getline(file, line))
	    	body += line + "\n";
        
	    std::stringstream ss ;
	    ss << body.size();
	    s = ss.str();
    }
	

	// std::string body, line;
	// while (getline(file, line))
	// 	body += line + "\n";

	// std::stringstream ss ;
	// ss << body.size();
	// std::string s = ss.str();

	std::string header =
		"Content-Type: text/html\r\n"
		"Content-Length: " + s + "\r\n"
		"Date: Sun, 10 Aug 2025 16:45:00 GMT\r\n"
		"Server: webserv42/1.0\r\n"
		"Connection: close\r\n";

	std::string response;
	response += statusLine;
	response += header;
	response += "\r\n";
	response += body;

	return response;
}
// void HandleClient(const int &fd_epoll, const int &fd_client, Config& a)
// {
//     // this just simple handling of client !!!!!!!

//     // so i get file that conation message of request : 

//     // it is fd_Client;
//     // so should parse it , for prepare responce !!!
//     Request test_request;
//     char buffer[1024];
//     int b_read = read(fd_client, buffer, 1024);
//     if (b_read > 0)
//     {
//         buffer[b_read] = '\0';
//         // std::cout << "Request is :\n\n";
//         std::cout << buffer << std::endl;
//         // if (info_location.get_path() == "None"
//         //     || check_request(info_location, test_request) == false)
//         //     std::cout << "Path not found or request not found\n";
//         // std::cout << "test request is " << test_request.get_path() << std::endl;
//         test_request.parse_request(buffer);
//         LocationConfig info_location = a.get_info_location(test_request.get_path());
//         std::string path = info_location.get_root() + info_location.get_path();
//         std::cout << "full path is : " << path << std::endl;
//         struct stat statbuf;
//         if (stat(path.c_str(), &statbuf) == 0)
//         {
//             if (S_ISDIR(statbuf.st_mode))
//             {
//                 if (info_location.get_pathIndex() == "None")
//                 {
//                     if (info_location.get_autoIndex() == false && info_location.get_pathIndex() == "None")
//                         std::cout << "autoindex is off and index not found\n";
//                     else
//                     {
                        

//                         //test response
//                         std::string response;
                        
//                         std::string body;
//                         body += "<html>\n";
//                         body += "<head><title>Index of ";
//                         body += info_location.get_path();
//                         body += "</title></head>\n";
//                         body += "<body>\n";
//                         body += "<h1>Index of /images/</h1>\n";
//                         body += "<ul>\n";
//                         struct dirent* entry;
//                         while ((entry = readdir(dir)))
//                         {
//                             if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
//                             {
//                                 body += "    <li><a href=\"" + std::string(entry->d_name) + "\">";
//                                 body += std::string(entry->d_name) + "</a></li>\n";
//                             }
//                         }
//                         body += "</ul>\n";
//                         body += "</body>\n";
//                         body += "</html>\n";
//                         closedir(dir);
//                         std::ostringstream oss;
//                         oss << body.size();
//                         response = "HTTP/1.1 200 OK\r\n";
//                         response += "Content-Type: text/html\r\n";
//                         response += "Content-Length: " + oss.str() + "\r\n";
//                         response += "\r\n";
//                         response += body;

//                         send(fd_client, response.c_str(), response.size(), 0);
//                     }
//                     // if auto_index is on dislpay all the current files and dir
//                 }
//                 else
//                 {
//                     std::string response;
                        
//                     std::string body;
//                     body += "<html>\n";
//                     body += "<head><title>Index of ";
//                     body += info_location.get_path();
//                     body += "</title></head>\n";
//                     body += "<body>\n";
//                     body += "<h1>Index of /images/</h1>\n";
//                     body += "<ul>\n";
//                     body += "</ul>\n";
//                     body += "</body>\n";
//                     body += "</html>\n";
//                     std::ostringstream oss;
//                     oss << body.size();
//                     response = "HTTP/1.1 200 OK\r\n";
//                     response += "Content-Type: text/html\r\n";
//                     response += "Content-Length: " + oss.str() + "\r\n";
//                     response += "\r\n";
//                     response += body;
//                     send(fd_client, response.c_str(), response.size(), 0);
//                     // do response here
//                 }
//             }
//             else
//             {
//                 std::string response;
                        
//                 std::string body;
//                 body += "<html>\n";
//                 body += "<head><title>Index of ";
//                 body += info_location.get_path();
//                 body += "</title></head>\n";
//                 body += "<body>\n";
//                 body += "<h1>Index of ";
//                 body += info_location.get_path();
//                 body += "</h1>\n";
//                 body += "<ul>\n";
//                 body += "</ul>\n";
//                 body += "</body>\n";
//                 body += "</html>\n";
//                 std::ostringstream oss;
//                 oss << body.size();
//                 response = "HTTP/1.1 200 OK\r\n";
//                 response += "Content-Type: text/html\r\n";
//                 response += "Content-Length: " + oss.str() + "\r\n";
//                 response += "\r\n";
//                 response += body;
//                 send(fd_client, response.c_str(), response.size(), 0);
//             }
//         }
//         else
//         {
//             std::cout << "Not found\n";
//         }
//         // std::cout << "path location is " << info_location.get_path() << std::endl;
//     }
//     else
//     {
//         epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd_client, NULL);
//         std::cout << "cant read from the file ..." << std::endl;
//         close(fd_client);
//     }
// }