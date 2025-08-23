#include "../../INCLUDES/Webserv.hpp"

User::User(): username(""), password("")
{
	//username = "";
	//password = "";
}

User::User(const std::string &u, const std::string &p)
	:username(u), password(p)
{}

bool User::operator==(const User &other)
{
	return (this->password == other.password && this->username == other.username);
}

std::string User::getPassword() const
{
	return password;
}

std::string User::getUsername() const
{
	return username;
}

void User::setPassword(std::string p)
{
	password = p;
}

void User::setUsername(std::string u)
{
	username = u;
}

//void Get_all_users()
//{
//	std::string username;
//	std::string pass;
//	int i = 0;
//	std::string line;

//	std::ifstream file("DATA/data.txt");
//	if (!file.is_open())
//		throw(std::runtime_error("cannot connect database !"));

//	while (true)
//	{
//		getline(file, line);
//		//std::cout << line << "--" << line.size() << std::endl;
//		if (line.empty())
//			break;
//		while (line[i] != '&')
//		{
//			username += line[i];
//			i++;
//		}
//		i++;
//		while (line[i])
//		{
//			pass += line[i];
//			i++;
//		}
//		users.push_back(User(username, pass));
//		username = "";
//		pass = "";
//		line = "";
//	}
//}

void Request::save_user_in_data()
{
	std::ofstream file("DATA/data.txt", std::ios::app);
	if (!file.is_open())
		throw(std::runtime_error("connot connect database !"));
	if (!user.getUsername().empty() && !user.getPassword().empty())
		file << user.getUsername() << "&" << user.getPassword() << std::endl;
}

bool Request::check_if_user_exist()
{
	std::ifstream file("DATA/data.txt");
	if (!file.is_open())
		throw(std::runtime_error("cannot connect database !"));
	std::string line;

	while (std::getline(file, line))
	{
		//std::cout << line << "--" << line.size() << std::endl;
		if (line.empty())
			continue;
		size_t pos = line.find('&');
		if (pos == std::string::npos)
			continue;
		std::string uname = line.substr(0, pos);
		std::string password = line.substr(pos + 1);
		User u(uname, password);
		if (user == u)
			return true;
	}
	return false;
}

//void User::get_user_form_request(const std::string& str)
//{
//	size_t pos;
//	std::string u;
//	std::string p;

//	pos = str.find("username=");
//	if (pos != std::string::npos)
//	{
//		pos += 9;
//		size_t end =  str.find('&', pos);
//		u = str.substr(pos, end - pos);
//	}
//	pos = str.find("password=");
//	if (pos != std::string::npos)
//	{
//		pos += 9;
//		size_t end =  str.find("\r\n", pos);
//		p = str.substr(pos, end - pos);
//	}
//	if (!u.empty() && !p.empty())
//	{
//		this->setPassword(p);
//		this->setUsername(u);	
//	}
//}