#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdio>
#include <algorithm>
#include <string_view>
#include <vector>

class Logger
{
public:
    virtual ~Logger() {}

    virtual void info(const std::string_view &module, const std::string_view &msg) = 0;
    virtual void warning(const std::string_view &module, const std::string_view &msg) = 0;
    virtual void error(const std::string_view &module, const std::string_view &msg) = 0;

    virtual void addInfoFP(FILE *fp)
    {
        if(fp != NULL)
            m_infoFP.push_back(fp);
    }
    virtual void addWarningFP(FILE *fp)
    {
        if(fp != NULL)
            m_warnFP.push_back(fp);
    }
    virtual void addErrorFP(FILE *fp)
    {
        if(fp != NULL)
            m_errorFP.push_back(fp);
    }
protected:
    std::vector<FILE*> m_infoFP;
    std::vector<FILE*> m_warnFP;
    std::vector<FILE*> m_errorFP;
};

#endif // LOGGER_HPP
