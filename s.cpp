#include <iostream>

int ree(std::string ori, std::string check)
{
  int i = 0;
  while (ori[i] &&  check[i] && ori[i] == check[i])
  {
    i++;
  }
  if (check[i] && ori[i])
    return -1;
  if (check[i - 1] != '/' || ori[i - 1] != '/')
    return -1;
  return i;
  
}
int main()
{
  std::string str1 = "/static/";
  std::string str2 = "/static/images/";
  std::string str3 = "/";
  
  std::string test = "/static/images/icons/icon.png";
  
  std::cout << ree(test, str1) << std::endl;
  std::cout << ree(test, str2) << std::endl;
  std::cout << ree(test, str3) << std::endl;
  return 0;
}