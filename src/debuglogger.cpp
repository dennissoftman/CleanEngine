#include "debuglogger.hpp"
#include <cstdio>
#include <ctime>

inline std::string get_timestamp()
{
    const char *months[] = {
        "Jan", "Feb",
        "Mar", "May", "Apr",
        "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov",
        "Dec"
    };

    time_t dt = std::time(0);
    struct tm *ts = std::localtime(&dt);
    char buff[64];
    sprintf(buff, "%d %s %2d:%02d:%02d",
            1900+ts->tm_year,
            months[ts->tm_mon%12],
            ts->tm_hour, ts->tm_min, ts->tm_sec);
    return std::string(buff);
}

void DebugLogger::info(const std::string_view &module, const std::string_view &msg)
{
    if(m_infoFP != NULL)
        fprintf(m_infoFP, "%s INFO [%s] %s\n", get_timestamp().c_str(), module.data(), msg.data());
}

void DebugLogger::warning(const std::string_view &module, const std::string_view &msg)
{
    if(m_warnFP != NULL)
        fprintf(m_warnFP, "%s WARNING [%s] %s\n", get_timestamp().c_str(), module.data(), msg.data());
}

void DebugLogger::error(const std::string_view &module, const std::string_view &msg)
{
    if(m_errorFP != NULL)
        fprintf(m_errorFP, "%s ERROR [%s] %s\n", get_timestamp().c_str(), module.data(), msg.data());
}
