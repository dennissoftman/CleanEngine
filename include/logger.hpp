#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdio>
#include <algorithm>
#include <string_view>

class Logger
{
public:
    virtual ~Logger() {}

    virtual void info(const std::string_view &module, const std::string_view &msg) = 0;
    virtual void warning(const std::string_view &module, const std::string_view &msg) = 0;
    virtual void error(const std::string_view &module, const std::string_view &msg) = 0;

    virtual void setInfoFP(FILE *fp)
    {
        if(fp != NULL)
            m_infoFP = fp;
    }
    virtual void setWarningFP(FILE *fp)
    {
        if(fp != NULL)
            m_warnFP = fp;
    }
    virtual void setErrorFP(FILE *fp)
    {
        if(m_errorFP != NULL)
            m_errorFP = fp;
    }
protected:
    FILE *m_infoFP = stdout;
    FILE *m_warnFP = stdout;
    FILE *m_errorFP = stderr;
};

#endif // LOGGER_HPP
