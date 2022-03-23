#ifndef SERVICELOCATOR_HPP
#define SERVICELOCATOR_HPP

#include "logger.hpp"
#include "dummylogger.hpp"

class ServiceLocator
{
public:
    static void init();

    // logging
    static Logger &getLogger();
    static void setLogger(Logger *logger);

    // unregister all services
    static void clear();
private:
    // logging
    static Logger *m_logger;
    static DummyLogger m_defaultLogger;

    //
};

#endif // SERVICELOCATOR_HPP
