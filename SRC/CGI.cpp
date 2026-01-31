#include "../INCLUDES/CGI.hpp"
#include <sstream>

CGI::CGI(const char *RequestType,
		const char *FilePath,
		const char *Language,
		const char *CgiBinary,
		const char **Env) : requestType(RequestType),
		Filepath(FilePath), language(Language),
		cgiBinary(CgiBinary), env(Env)
{
	execArg = new char*[3];
	execArg[0] = const_cast<char *>(cgiBinary);
	execArg[1] = const_cast<char *>(Filepath);
	execArg[2] = NULL;
	file.open("./SRC/temp/temp.txt", std::ios::out);
}

bool CGI::CGIProccess()
{
	int lvl = -1;
	std::string s = this->language;
	std::string supported[2] = {".php", ".py"}; // addable..
	for (int i = 0; i < NSCGI; i++)
	{
		if (s == supported[i])
		{
			lvl = i;
			break;
		}
	}
	switch (lvl){
		case PHP:
			break;
		case PYTHON:
			break;
		default:
			return false;
	}
	if (CGIOutput())
		return false;
	responseWrapper();
	return true;
}

int CGI::wrireToFile(std::ofstream &file, char *str)
{
    if (!str)
        return std::cout << "didn't write to file\n", 1;
    file <<  str;
    return 0;
}

int CGI::CGIOutput()
{
    pid_t id;
    int status;
    int piped[2];
    std::ofstream file("temp.txt");

    if (pipe(piped) == -1)
        return std::cerr << "pipe failed\n", 1;
    id = fork();
    if (id == -1)
        return std::cerr << "fork failed\n", 1;
    if (id == 0)
    {
        close(piped[0]);
        dup2(piped[1], 1);
        close(piped[1]);
        execve(cgiBinary, execArg, (char **)env);
        return 1;
    }
    close(piped[1]);

    // Make pipe non-blocking
    int flags = fcntl(piped[0], F_GETFL, 0);
    fcntl(piped[0], F_SETFL, flags | O_NONBLOCK);

    char buffer[4096];
    int totalRead = 0;
    int bytesRead;

    int timed_out = 0;
    time_t start = time(NULL);
    const int timeout_sec = 5;

    while (1) {
        while ((bytesRead = read(piped[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            file << buffer;
            totalRead += bytesRead;
        }
        pid_t result = waitpid(id, &status, WNOHANG);
        if (result == id) {
            break;
        }
        if (difftime(time(NULL), start) >= timeout_sec) {
            timed_out = 1;
            kill(id, SIGKILL);
            waitpid(id, &status, 0);
            break;
        }
        usleep(100000);
    }

    if (timed_out) {
        close(piped[0]);
        file.close();
        std::cerr << "CGI script timed out after 5 seconds\n";
        return 1;
    }

    if (totalRead == 0)
        return std::cerr << "couldn't read CGI output\n", 1;

    close(piped[0]);
    file.close();
    return 0;
}

int CGI::responseWrapper()
{
	std::string statusLine = "HTTP/1.1 200 OK\r\n";
	std::ifstream file("temp.txt");
	std::stringstream output;
	output << file.rdbuf();
	std::string lines = output.str();
	file.close();
	
	std::cout << "---lines---\n";
	std::cout << lines << std::endl;

	if (lines.find("Content-Type:") != lines.npos)
	{
		response = statusLine + lines;
	}
	else
	{
		std::ostringstream contentLengthStream;
		contentLengthStream << lines.length();
		std::string header =
			"Content-Type: text/html\r\n"
			"Content-Length: " + contentLengthStream.str() + "\r\n\r\n";

		response = statusLine + header + lines;
	}
	std::cout << "----response----\n" << response << std::endl;
	return 0;
}

CGI::~CGI()
{
	delete[] execArg;
}