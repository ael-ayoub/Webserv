#include "../../INCLUDES/Webserv.hpp"



int Methodes::PostMethod(const std::string& Prequest, const LocationConfig& info_location)
{
	if (Prequest.empty())
		throw(std::invalid_argument("Post request is empty !"));

	if (info_location.is_upload_enabled()) {
		Upload uploader(Prequest);
		return 0;
	}

	std::string path = info_location.get_root() + info_location.get_path();
	if (path.size() > 3 && (path.substr(path.size()-3) == ".py" || path.substr(path.size()-4) == ".php")) {
		// Example: you may need to extract env, cgiBinary, etc. from config
		const char* reqType = "POST";
		const char* filePath = path.c_str();
		const char* lang = (path.substr(path.size()-3) == ".py") ? ".py" : ".php";
		const char* cgiBinary = (lang == std::string(".py")) ? "/usr/bin/python3" : "/usr/bin/php-cgi";
		std::cout << "\n\n\n\n\n-----cgi bin = " << cgiBinary << "----\n\n\n\n\n\n\n";
		const char* env[] = { NULL };
		CGI cgi(reqType, filePath, lang, cgiBinary, env);
		if (cgi.CGIProccess()) {
			// return cgi.response or status
			return 0;
		} else {
			return -1;
		}
	}
	std::string fallback_path = info_location.get_root() + "/uploads/filename.txt";
	std::ofstream fileUpload(fallback_path.c_str(), std::ios::app);
	if (!fileUpload.is_open())
		throw(std::invalid_argument("cannt create file !!"));

	size_t boundary_satart = Prequest.find("-----");
	size_t boundary_end = Prequest.find("\r\n\r\n");
	std::string boundary = Prequest.substr(boundary_satart, boundary_end);
	std::cout << boundary << std::endl;
	size_t end = Prequest.find(boundary, boundary_end);
	std::string body  = Prequest.substr(boundary_end + 5, end);
	std::cout << "*******************" << std::endl;
	std::cout << body << std::endl;
	fileUpload << body;
	std::cout << "*******************" << std::endl;
	return (1);
}
