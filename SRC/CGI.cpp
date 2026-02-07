#include "../INCLUDES/CGI.hpp"
#include <sstream>

CGI::CGI(const char *RequestType,
		const char *FilePath,
		const char *Language,
		const char *CgiBinary,
		const char **Env) : requestType(RequestType),
		Filepath(FilePath), language(Language),
        cgiBinary(CgiBinary), env(Env), timed_out(false)
{
	execArg = new char*[3];
	execArg[0] = const_cast<char *>(cgiBinary);
	execArg[1] = const_cast<char *>(Filepath);
	execArg[2] = NULL;
	file.open("./SRC/temp/temp.txt", std::ios::out);
}

bool CGI::CGIProccess()
{
    if (!cgiBinary || !*cgiBinary || !Filepath || !*Filepath)
        return false;
    if (CGIOutput())
    {
        return false;
    }
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
        this->timed_out = true;
        close(piped[0]);
        file.close();
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

    // Normalize CGI output to CRLF so HTTP clients parse headers reliably.
    std::string normalized;
    normalized.reserve(lines.size() + 16);
    for (size_t i = 0; i < lines.size(); i++)
    {
        if (lines[i] == '\n')
        {
            if (i == 0 || lines[i - 1] != '\r')
                normalized += "\r\n";
            else
                normalized += "\n";
        }
        else
        {
            normalized += lines[i];
        }
    }

    if (normalized.find("Content-Type:") != normalized.npos)
    {
        size_t sep = normalized.find("\r\n\r\n");
        if (sep == std::string::npos)
        {
            // If CGI didn't produce a valid header/body separator, treat as body.
            std::ostringstream contentLengthStream;
            contentLengthStream << normalized.length();
            response = statusLine +
                "Content-Type: text/html\r\n"
                "Content-Length: " + contentLengthStream.str() + "\r\n"
                "Connection: close\r\n\r\n" +
                normalized;
            return 0;
        }

        std::string cgi_headers = normalized.substr(0, sep);
        std::string cgi_body = normalized.substr(sep + 4);
        bool has_content_length = (cgi_headers.find("Content-Length:") != std::string::npos);
        bool has_connection = (cgi_headers.find("Connection:") != std::string::npos);

        std::string extra;
        if (!has_content_length)
        {
            std::ostringstream contentLengthStream;
            contentLengthStream << cgi_body.length();
            extra += "Content-Length: " + contentLengthStream.str() + "\r\n";
        }
        if (!has_connection)
            extra += "Connection: close\r\n";

        response = statusLine + cgi_headers + "\r\n" + extra + "\r\n" + cgi_body;
    }
	else
	{
		std::ostringstream contentLengthStream;
        contentLengthStream << normalized.length();
		std::string header =
			"Content-Type: text/html\r\n"
			"Content-Length: " + contentLengthStream.str() + "\r\n\r\n";

        response = statusLine + header + normalized;
	}
	return 0;
}

CGI::~CGI()
{
	delete[] execArg;
}