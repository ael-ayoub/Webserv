#ifndef USERS_HPP
#define USERS_HPP

#include "Webserv.hpp"

class User
{
	private:
		std::string username;
		std::string password;
	public:
		User();
		User(const std::string& u, const std::string& p);
		std::string getPassword() const;
		std::string getUsername() const;
		void setUsername(std::string u); 
		void setPassword(std::string p);
		bool operator==(const User& other);

		//void get_user_form_request(const std::string& str);

};


//std::vector<User> Get_all_users();
//void save_user_in_data(const User& u);
//User get_user_form_request(const std::string& str);
//bool check_if_user_exist(const User &u);


#endif 

