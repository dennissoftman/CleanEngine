#ifndef DEBUGLOGGER_HPP
#define DEBUGLOGGER_HPP

#include "logger.hpp"

class DebugLogger : public Logger
{
public:
    void info(const std::string_view &module, const std::string_view &msg) override;
    void warning(const std::string_view &module, const std::string_view &msg) override;
    void error(const std::string_view &module, const std::string_view &msg) override;
};

#endif // DEBUGLOGGER_HPP
