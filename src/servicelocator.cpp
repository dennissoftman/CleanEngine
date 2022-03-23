#include "servicelocator.hpp"
#include <memory>

DummyLogger ServiceLocator::m_defaultLogger = DummyLogger();
Logger *ServiceLocator::m_logger = nullptr;

void ServiceLocator::init()
{
    m_logger = &m_defaultLogger;
}

Logger &ServiceLocator::getLogger()
{
    return *m_logger;
}

void ServiceLocator::setLogger(Logger *logger)
{
    if(logger != nullptr)
        m_logger = logger;
}

void ServiceLocator::clear()
{
    if(m_logger != &m_defaultLogger)
        delete m_logger;
}
