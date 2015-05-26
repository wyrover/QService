
#ifndef Q_DBLOGER_H_
#define Q_DBLOGER_H_

#include "Loger.h"
#include "QMySQL.h"
#include "Mutex.h"
#include "TcpParser.h"
#include "Singleton.h"

//数据库日志
class CDBLoger : public CLoger,
    public CSingleton<CDBLoger>
{
public:
    CDBLoger(void);
    ~CDBLoger(void);

    //初始化
    bool Init(const char *pszIp, unsigned short &usPort, 
        const char *pszUser, const char *pszPWD, const char *pszDB);

    //写
    void writeDBLog(const char *pszPlayerID, const short sType, 
        const char *pszMsg, const size_t iLens);

public:
    //接口
    void Write(const char *pszMsg, const size_t iLens);
    bool Check(void);

private:
    bool Link(void);
    std::string getTableNam(void) const;
    bool createTable(const std::string &strName);

private:
    CDBStatement *m_pStatement;
    std::string m_strTable;
    CMySQLLink m_objLinker;
    CDBUrl m_objUrl;
    CQMutex m_objMutex;
    CTcpParser m_objTcpParser;        
};

#endif//Q_DBLOGER_H_
