#ifndef CGI_HPP
#define CGI_HPP

#include "includes.hpp"

#define PHP 0
#define PYTHON 1
#define NSCGI 2 // edit this later

class CGI{
	protected:
		const char *requestType;
		const char *Filepath;
		const char *language;
		std::ofstream file;
		const char *cgiBinary;
		const char **env;
		char **execArg;
		bool timed_out;
		int wrireToFile(std::ofstream &file, char *str);
		int CGIOutput();
		int responseWrapper();
	public:
		std::string response;
		CGI(const char *, const char *, const char *, const char *, const char **);
		bool CGIProccess();
		bool TimedOut() const { return timed_out; }
		~CGI();
};

#endif