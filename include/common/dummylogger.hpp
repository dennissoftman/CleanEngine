#ifndef DUMMYLOGGER_HPP
#define DUMMYLOGGER_HPP

#include "common/logger.hpp"

class DummyLogger : public Logger
{
public:
    DummyLogger();

    void info(const std::string_view &module, const std::string_view &msg) override;
    void warning(const std::string_view &module, const std::string_view &msg) override;
    void error(const std::string_view &module, const std::string_view &msg) override;
};

#endif // DUMMYLOGGER_HPP
