
#ifndef Q_SYSLOG_H_
#define Q_SYSLOG_H_

#include "Macros.h"

/*
系统日志,直接记录进文件
*/
class CSysLoger
{
public:
    CSysLoger(void);
    ~CSysLoger(void);

    /*记录日志*/
    void sysLog(const LOG_LEVEL emInLogLv,
        const char *pFile, const char *pFunction, const int iLine,
        const char *pFormat, ...);

private:
    std::string getLV(LOG_LEVEL emInLogLv) const;

private:
    FILE *m_pFile;
    class CQMutex *m_pMutex;
};

#endif//Q_SYSLOG_H_
