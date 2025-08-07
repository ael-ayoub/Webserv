#include "../../INCLUDES/Syntax_location.hpp"

void syntax_location::check_locations(Vector_str str, int *i)
{
    size_t store_i = *i;
    // int brace_cout = 0;

    while (store_i < str.size())
    {
        Vector_str tmp = ServerConfig::ft_splitv2(ServerConfig::remove_spaces(str[store_i]), ' ');
        size_t j = 0;
        if ((tmp.size() < 3 || tmp[2] != "{") && tmp[0] == "location")
            throw Config::ErrorSyntax();
        // if (tmp.size() == 1 && tmp[0][0] == '}')
        //     break;
        while (j < tmp.size())
        {
            std::cout << tmp[j] << "...";
            j++;
        }
        std::cout << std::endl;
        if (tmp[0] == "methods")
            tmp[tmp.size() - 1].erase(tmp[tmp.size() - 1][tmp[tmp.size() - 1].size() - 1]);
            // tmp[tmp.size() - 1][tmp[tmp.size() - 1]]
        if (tmp[0] == "upload_store" || (tmp[0] == "methods" && tmp[1] == "POST"))
            check_upload(str, i);
        // std::cout << "tmp 1 is " << tmp[1] << std::endl;
        store_i++;
    }
}

void syntax_location::check_upload(Vector_str str, int *i)
{
    (void)str, (void)i;
    std::cout << "found upload\n";
}
