#include "../../INCLUDES/Webserv.hpp"



// Integrate Upload and CGI handling
int Methodes::PostMethod(const std::string& Prequest, const LocationConfig& info_location)
{
	if (Prequest.empty())
		throw(std::invalid_argument("Post request is empty !"));

	// Check if upload is enabled for this location
	if (info_location.is_upload_enabled()) {
		Upload uploader(Prequest);
		// Optionally, you can return a status or response string
		return 0; // Success
	}

	// Check if this is a CGI request (by file extension in path)
	std::string path = info_location.get_root() + info_location.get_path();
	if (path.size() > 3 && (path.substr(path.size()-3) == ".py" || path.substr(path.size()-4) == ".php")) {
		// Example: you may need to extract env, cgiBinary, etc. from config
		const char* reqType = "POST";
		const char* filePath = path.c_str();
		const char* lang = (path.substr(path.size()-3) == ".py") ? ".py" : ".php";
		const char* cgiBinary = (lang == std::string(".py")) ? "/usr/bin/python3" : "/usr/bin/php-cgi";
		const char* env[] = { NULL };
		CGI cgi(reqType, filePath, lang, cgiBinary, env);
		if (cgi.CGIProccess()) {
			// Optionally, you can return cgi.response or status
			return 0;
		} else {
			// CGI failed
			return -1;
		}
	}

	// Fallback: old logic (write to file)
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
