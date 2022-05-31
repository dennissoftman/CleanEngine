#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdio>
#include <string_view>
#include <vector>

class Logger
{
public:
    virtual ~Logger() {}

    virtual void info(const std::string_view &module, const std::string_view &msg) = 0;
    virtual void warning(const std::string_view &module, const std::string_view &msg) = 0;
    virtual void error(const std::string_view &module, const std::string_view &msg) = 0;

    void addInfoFP(FILE *fp);
    void addWarningFP(FILE *out);
    void addErrorFP(FILE *out);
protected:
    std::vector<FILE*> m_infoFP;
    std::vector<FILE*> m_warnFP;
    std::vector<FILE*> m_errorFP;
};

#endif // LOGGER_HPP
