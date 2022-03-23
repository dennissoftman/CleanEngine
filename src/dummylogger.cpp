#include "dummylogger.hpp"

DummyLogger::DummyLogger()
{

}

void DummyLogger::info(const std::string_view &module, const std::string_view &msg)
{
    (void)module;
    (void)msg;
}

void DummyLogger::warning(const std::string_view &module, const std::string_view &msg)
{
    (void)module;
    (void)msg;
}

void DummyLogger::error(const std::string_view &module, const std::string_view &msg)
{
    (void)module;
    (void)msg;
}
