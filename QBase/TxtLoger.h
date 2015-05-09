
#ifndef Q_TXTLOGERER_H_
#define Q_TXTLOGERER_H_

#include "Loger.h"
#include "Mutex.h"
#include "TcpParser.h"

//文本日志
class CTxtLoger : public CLoger
{
public:
    CTxtLoger(void);
    ~CTxtLoger(void);

    /*设置打印级别，低于该级别的不输出, 设置LOGLV_NOLOG不输出日志*/
    void setPriority(const LOG_LEVEL emLV);
    /*设置日志文件*/
    void setLogFile(const char *pLogFile);
    /*设置日志文件大小*/
    void setLogMaxSize(const size_t iSize);

    void writeLog(const LOG_LEVEL emInLogLv,
        const char *pFile, const char *pFunction, const int iLine, const Q_SOCK &fd,
        const char *pFormat, ...);

    void Open(void);

public:
    void Write(const char *pszMsg, const size_t iLens);

private:
    std::string getLV(LOG_LEVEL emInLogLv) const;

private:
    FILE *m_pFile;
    LOG_LEVEL m_emPriorityLV;
    size_t m_uiLogSize;
    time_t m_LastCheckTime;
    std::string m_strFilePath;
    CQMutex m_objMutex;
    CTcpParser m_objTcpParser;
};

#endif//Q_TXTLOGERER_H_
