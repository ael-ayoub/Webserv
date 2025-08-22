#include "../../INCLUDES/Webserv.hpp"

int Methodes::PostMethod(const std::string& Prequest, const LocationConfig& info_location)
{
	if (Prequest.empty())
	{
		std::cerr << "Post request is empty!" << std::endl;
		return -1;
	}

	if (info_location.is_upload_enabled()) {
		try {
			Upload uploader(Prequest);
			return 0;
		} catch (const std::exception& e) {
			std::cerr << "Upload error: " << e.what() << std::endl;
			return -1;
		}
	}

	std::string path = info_location.get_root() + info_location.get_path();
	if (path.size() > 3 && (path.substr(path.size()-3) == ".py" || path.substr(path.size()-4) == ".php")) {
		try {
			const char* reqType = "POST";
			const char* filePath = path.c_str();
			const char* lang = (path.substr(path.size()-3) == ".py") ? ".py" : ".php";
			const char* cgiBinary = (lang == std::string(".py")) ? "/usr/bin/python3" : "/usr/bin/php-cgi";
			std::cout << "\n\n\n\n\n-----cgi bin = " << cgiBinary << "----\n\n\n\n\n\n\n";
			const char* env[] = { NULL };
			CGI cgi(reqType, filePath, lang, cgiBinary, env);
			if (cgi.CGIProccess()) {
				return 0;
			} else {
				return -1;
			}
		} catch (const std::exception& e) {
			std::cerr << "CGI processing error: " << e.what() << std::endl;
			return -1;
		}
	}
	std::string fallback_path = info_location.get_root() + "/uploads/filename.txt";
	std::ofstream fileUpload(fallback_path.c_str(), std::ios::app);
	if (!fileUpload.is_open())
	{
		std::cerr << "Cannot create file: " << fallback_path << std::endl;
		return -1;
	}

	size_t boundary_start = Prequest.find("-----");
	size_t boundary_end = Prequest.find("\r\n\r\n");
	if (boundary_start == std::string::npos || boundary_end == std::string::npos) {
		std::cerr << "Invalid multipart request format" << std::endl;
		fileUpload.close();
		return -1;
	}
	if (boundary_end <= boundary_start) {
		std::cerr << "Invalid boundary positions" << std::endl;
		fileUpload.close();
		return -1;
	}
	
	std::string boundary = Prequest.substr(boundary_start, boundary_end - boundary_start);
	std::cout << boundary << std::endl;
	
	size_t end = Prequest.find(boundary, boundary_end);
	if (end == std::string::npos) {
		std::cerr << "Could not find end boundary" << std::endl;
		fileUpload.close();
		return -1;
	}
	if (boundary_end + 5 >= end) {
		std::cerr << "Not enough content between boundaries" << std::endl;
		fileUpload.close();
		return -1;
	}
	
	std::string body = Prequest.substr(boundary_end + 4, end - (boundary_end + 4));
	std::cout << "*******************" << std::endl;
	std::cout << body << std::endl;
	fileUpload << body;
	fileUpload.close();
	std::cout << "*******************" << std::endl;
	return 0;
}
