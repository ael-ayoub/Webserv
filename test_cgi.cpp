#include "INCLUDES/Response.hpp"
#include "INCLUDES/Config.hpp"
#include "INCLUDES/LocationConfig.hpp"
#include "INCLUDES/Request.hpp"

int main()
{
    Config config;
    LocationConfig location;
    Request request;
    
    
    std::cout << "Testing Python CGI:\n";
    std::string python_response = Response::Get_response("./STATIC/hello.py", location, request, config);
    std::cout << "Python Response Length: " << python_response.length() << "\n";
    
      
    std::cout << "Testing PHP CGI:\n";
    std::string php_response = Response::Get_response("./STATIC/hello.php", location, request, config);
    std::cout << "PHP Response Length: " << php_response.length() << "\n";
    
    
    std::cout << "Testing regular file:\n";
    std::string regular_response = Response::Get_response("./STATIC/index.html", location, request, config);
    std::cout << "Regular Response Length: " << regular_response.length() << "\n";
    
    return 0;
}
