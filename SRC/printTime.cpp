#include "../INCLUDES/includes.hpp"

void printCurrentTime()
{
    std::time_t now = std::time(nullptr);
    std::tm *lt = std::localtime(&now);

    static const char* months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    std::cout << "["
              << std::setw(2) << std::setfill('0') << lt->tm_mday << "/"
              << months[lt->tm_mon] << "/"
              << (lt->tm_year + 1900) << " "
              << std::setw(2) << std::setfill('0') << lt->tm_hour << ":"
              << std::setw(2) << std::setfill('0') << lt->tm_min << ":"
              << std::setw(2) << std::setfill('0') << lt->tm_sec
              << "]\n";
}
