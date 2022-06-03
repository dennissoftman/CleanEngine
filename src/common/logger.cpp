#include "common/logger.hpp"

void Logger::addInfoFP(FILE *fp)
{
    m_infoFP.push_back(fp);
}

void Logger::addWarningFP(FILE *fp)
{
    m_warnFP.push_back(fp);
}

void Logger::addErrorFP(FILE *fp)
{
    m_errorFP.push_back(fp);
}
