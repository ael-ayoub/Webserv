#ifndef UPLOAD_HPP
#define UPLOAD_HPP

#include "includes.hpp"

class Upload
{
        const std::string request;
        std::string boundary;
        std::vector<size_t> contentLenght;
        std::vector<std::string> filename;
        std::vector<std::string> files;
		std::vector<std::string> chunks;
    public:
		void normalUpload();
		Upload(std::string Request);
        bool ReqChecker();
		void fileContent();
		void appendContentToLocalFile();
        bool getData();
		void printChunks();

};


#endif