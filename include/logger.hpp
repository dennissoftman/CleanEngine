#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdio>
#include <algorithm>

class Logger
{
public:
    ~Logger()
    {
        if(m_infoFP != NULL && m_infoFP != stdout && m_infoFP != stderr)
            fclose(m_infoFP);
        if(m_warnFP != NULL && m_warnFP != stdout && m_warnFP != stderr && m_warnFP != m_infoFP)
            fclose(m_warnFP);
        if(m_errorFP != NULL && m_errorFP != stdout && m_errorFP != stderr && m_errorFP != m_warnFP && m_errorFP != m_infoFP)
            fclose(m_errorFP);
    }

    virtual void info(const std::string_view &module, const std::string_view &msg) = 0;
    virtual void warning(const std::string_view &module, const std::string_view &msg) = 0;
    virtual void error(const std::string_view &module, const std::string_view &msg) = 0;

    void setInfoFP(FILE *fp) { m_infoFP = fp; }
    void setWarningFP(FILE *fp) { m_warnFP = fp; }
    void setErrorFP(FILE *fp) { m_errorFP = fp; }
protected:
    FILE *m_infoFP = stdout;
    FILE *m_warnFP = stdout;
    FILE *m_errorFP = stderr;
};

#endif // LOGGER_HPP
