#include "../../INCLUDES/Upload.hpp"
#include <cstdlib>

Upload::Upload(std::string Request) : request(Request)
{
	try {
		if (request.empty()) {
			std::cerr << "Upload: Empty request" << std::endl;
			return;
		}
		
		if (getData() == false)
			normalUpload();
		fileContent();
		appendContentToLocalFile();
	} catch (const std::exception& e) {
		std::cerr << "Upload constructor error: " << e.what() << std::endl;
		throw;
	}
}

bool Upload::ReqChecker()
{
    return request.find("multipart/form-data", 0) != request.npos;
}

void Upload::printChunks()
{
	for (size_t i = 0; i < files.size(); i++)
		std::cout << "Chunk part " << i + 1 << ":\n"<< files[i] << std::endl;
}

// * POST /upload HTTP/1.1\r\n
// * Host: localhost\r\n
// * User-Agent: CustomUploader/1.0\r\n
// * Accept: */*\r\n
// * Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryA1B2C3D4E5\r\n
// * Content-Length: 612\r\n
// * \r\n

// * ------WebKitFormBoundaryA1B2C3D4E5\r\n
// TODO : part 1
// * Content-Disposition: form-data; name=\"username\"\r\n
// * \r\n
// * Ayoub_S\r\n


// * ------WebKitFormBoundaryA1B2C3D4E5\r\n

// TODO : part 2
// * Content-Disposition: form-data; name=\"myfile\""; filename=\"hello.txt\"\r\n
// * Content-Type: text/plain\r\n
// * \r\n
// * Hello world from my file!\r\n
// * This file has multiple lines of text.\r\n
// * Line 1: Example data\r\n
// * Line 2: Another example\r\n
// * Line 3: Final line in text file.\r\n
// * \r\n

// * ------WebKitFormBoundaryA1B2C3D4E5\r\n

// TODO : part 3
// * Content-Disposition: form-data; name=\"image\"; filename=\"picture.jpg\"\r\n
// * Content-Type: image/jpeg\r\n
// * \r\n
// * <binary JPEG image data would go here>\r\n
// * ÿØÿà..JFIF..................\r\n
// * (binary data continues...)\r\n
// * \r\n

// * ------WebKitFormBoundaryA1B2C3D4E5--\r\n

void debugger(std::string &str)
{
	std::cout << "debugger:\n" << str << "\n" << std::endl;
}

bool Upload::getData()
{
	size_t start, end;
	if (ReqChecker() == false)
		return false;

	start = request.find("boundary=", 0);
	end = request.find("\r", start);
	if (end == request.npos || start == request.npos)
		return false;
	start += 9;
	boundary = "--" + request.substr(start, end - (start));
	
	start = request.find("Content-Length: ", 0);
	end = request.find("\r", start);
	if (end == request.npos || start == request.npos)
		return false;
	start += 16; // Length of "Content-Length: "
	std::string number = request.substr(start, end - start);

	contentLenght.push_back(atoi(number.c_str()));

	int i = 0;
	while (1)
	{
		start = end;
		start = request.find(boundary, start);
		if (start == request.npos)
			break;
		start += boundary.size();
		end = request.find(boundary, start);
		std::string temp = request.substr(start, end - start);
		if (temp.empty() == false)
			chunks.push_back(temp);
		start = end ;
		i++;
	}
	chunks.erase(chunks.end());
    return true;
}

void Upload::fileContent()
{
	size_t start, end;
	for (size_t i = 0; i < chunks.size(); i++)
	{
		std::string toFind = "filename=";
		start = chunks[i].find(toFind, 0);
		if (start == std::string::npos)
		{
			toFind = "name=";
			start = chunks[i].find(toFind, 0);
			if (start == std::string::npos)
				return std::cout << "The file has no name nor filename\n", (void)0;
		}
		start += toFind.size();
		end = chunks[i].find("\r\n", start);
		if (end == std::string::npos)
			return ;
		std::string line = chunks[i].substr(start, end - start);
		std::string file;
		for (size_t j = 0; j < line.size(); j++)
		{
			if (line[j] != '"')
				file += line[j];
		}
		if (file.empty() == false && file[0] == '/')
		{
			start = file.find_last_of('/', 0);
			if (start == file.npos)
				return std::cout << "error in filename : " << file << std::endl, (void)0;
			file = file.substr(start + 1);
		}
		if (file.empty() == false)
			filename.push_back(file);
	}

	// ! CONTENT
	for (size_t i = 0; i < chunks.size(); i++)
	{
		std::string toFind = "\r\n\r\n";
		start = chunks[i].find(toFind, 0);
		if (start == toFind.npos)
			return std::cout << "Invalid file\n", (void)0;
		start += toFind.size();
		std::string line = chunks[i].substr(start);
		if (line.empty() == false)
			files.push_back(line);
	}

}

void Upload::appendContentToLocalFile()
{
	for (size_t i = 0; i < files.size() && i < filename.size(); i++)
	{
		// debugger(filename[i]);
		if (filename[i].empty()) {
			std::cerr << "Warning: Empty filename for file " << i << std::endl;
			continue;
		}
		
		std::string filepath = "./uploads/" + filename[i];
		std::ofstream file(filepath.c_str());
		if (!file.is_open()) {
			std::cerr << "Could not create file: " << filepath << std::endl;
			continue;
		}
		
		file << files[i];
		file.close();
		std::cout << "File uploaded: " << filepath << std::endl;
	}
}

// * POST /upload HTTP/1.1
// * Host: localhost:8080
// * Content-Type: application/octet-stream
// * Content-Disposition: attachment; filename="notes.txt"
// * Content-Length: 32

// * Hello, this is the file content!

void Upload::normalUpload()
{
	size_t start, end;
	std::string toFind = "filename=";
	start = request.find(toFind, 0);
	if (start == std::string::npos)
	{
		toFind = "name=";
		start = request.find(toFind, 0);
		if (start == std::string::npos)
			return std::cout << "The file has no name nor filename\n", (void)0;
	}
	start += toFind.size();
	end = request.find("\r\n", start);
	if (end == std::string::npos)
		return ;
	std::string line = request.substr(start, end - start);
	std::string file;
	for (size_t j = 0; j < line.size(); j++)
	{
		if (line[j] != '"')
			file += line[j];
	}
	if (file.empty() == false && file[0] == '/')
	{
		start = file.find_last_of('/', 0);
		if (start == file.npos)
			return std::cout << "error in filename : " << file << std::endl, (void)0;
		file = file.substr(start + 1);
	}
	if (file.empty() == false)
		filename.push_back(file);
	std::string begining = "\r\n\r\n";
	start = request.find(begining, 0);
	if (start == request.npos)
		return std::cout << "error, file \'"<< filename.front() 
						 << "\' is empty : " << std::endl, (void)0;
	start += begining.size();
	line = request.substr(start);
	if (line.empty() == false)
		files.push_back(line);
}