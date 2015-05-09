
#include "DBLoger.h"
#include "SysLog.h"
#include "InitSock.h"

#define LOG_TABLE "log"

CDBLoger::CDBLoger(void) : m_pStatement(NULL), 
    m_objLinker(), m_objUrl(), m_objMutex(), m_objTcpParser()
{
    m_strTable = getTableNam();
}

CDBLoger::~CDBLoger(void)
{
    try
    {
        if (NULL != m_pStatement)
        {
            m_pStatement->finalize();
            m_pStatement = NULL;
        }

        m_objLinker.close();
    }
    catch (...)
    {
    	
    }    
}

std::string CDBLoger::getTableNam(void) const
{
    time_t now = time(NULL);
    struct tm *pstTM =localtime(&now);

    return Q_FormatStr("%s_%d_%d", LOG_TABLE, pstTM->tm_year + 1900, pstTM->tm_mon + 1);
}

bool CDBLoger::Init(const char *pszIp, unsigned short &usPort, 
    const char *pszUser, const char *pszPWD, const char *pszDB)
{
    m_objUrl.Format("DB=%s;UserName=%s;PassWord=%s;Port=%d;Host=%s;", 
        pszDB, pszUser, pszPWD, usPort, pszIp);

    if (Link())
    {
        return createTable(m_strTable);
    }

    return false;
}

void CDBLoger::writeDBLog(const Q_SOCK &fd, const char *pszPlayerID, const short sType, 
    const char *pszMsg, const size_t iLens)
{
    if (Q_INVALID_SOCK == fd)
    {
        return;
    }

    int64_t iPlayerID = Q_ToNumber<int64_t>(pszPlayerID);

    size_t iBufferLens = sizeof(iPlayerID) + sizeof(sType) + iLens;
    size_t iHeadLens = Q_INIT_NUMBER;
    const char *pszHead = m_objTcpParser.createHead(iBufferLens, iHeadLens);

    m_objMutex.Lock();
    (void)Q_SockWrite(fd, pszHead, iHeadLens);
    (void)Q_SockWrite(fd, (const char *)&iPlayerID, sizeof(iPlayerID));
    (void)Q_SockWrite(fd, (const char *)&sType, sizeof(sType));
    if (0 != iLens
        && NULL != pszMsg)
    {
        (void)Q_SockWrite(fd, pszMsg, iLens);
    }
    m_objMutex.unLock();
}

bool CDBLoger::Link(void)
{
    try
    {
        m_objLinker.open(m_objUrl);
    }
    catch (CQException &e)
    {
        Q_Printf("%s", e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "%s", e.getErrorMsg());

        return false;
    }

    return true;
}

bool CDBLoger::Check(void)
{
    try
    {
        (void)m_objLinker.execDML(Q_FormatStr("select id from %s limit 1", LOG_TABLE).c_str());
    }
    catch(CQException &)
    {
        return false;
    }

    return true;
}

bool CDBLoger::createTable(const std::string &strName)
{
    try
    {
        (void)m_objLinker.execDML(Q_FormatStr("CREATE TABLE IF NOT EXISTS %s like %s;", 
            strName.c_str(), LOG_TABLE).c_str());
    }
    catch(CQException &e)
    {
        Q_Printf("%s", e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "%s", e.getErrorMsg());

        return false;
    }

    return true;
}

void CDBLoger::Write(const char *pszMsg, const size_t iLens)
{
    bool bError = false;
    int64_t iPlayerID = *((int64_t *)pszMsg);
    short sType = *((short *)(pszMsg + sizeof(iPlayerID)));
    const char *pszBuffer = pszMsg + sizeof(sType) + sizeof(iPlayerID);
    size_t iBufLens = iLens - (sizeof(sType) + sizeof(iPlayerID));

    //表名检查
    std::string strNewTable = getTableNam();
    if (strNewTable != m_strTable)
    {
        if (createTable(strNewTable))
        {
            if (NULL != m_pStatement)
            {
                m_pStatement->finalize();
                m_pStatement = NULL;
            }

            m_strTable = strNewTable;
        }
    }

    //执行
    try
    {
        if (NULL == m_pStatement)
        {
            m_pStatement = m_objLinker.compileStatement(Q_FormatStr("insert into %s(playerid,logtype, logtime, info) values (?,?,?,?);",
                m_strTable.c_str()).c_str());
        }

        m_pStatement->bindInt64(0, iPlayerID);
        m_pStatement->bindInt(1, sType);
        m_pStatement->bindInt64(2, time(NULL));
        m_pStatement->bindBlob(3, (const unsigned char *)pszBuffer, iBufLens);

        (void)m_pStatement->execDML();
    }
    catch(CQException &e)
    {
        Q_Printf("open mysql link error. code %d, messgae %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "open mysql link error. code %d, messgae %s",
            e.getErrorCode(), e.getErrorMsg());

        bError = true;
    }

    if (!bError)
    {
        return;
    }
    if (Check())
    {
        return;
    }

    if (NULL != m_pStatement)
    {
        m_pStatement->finalize();
        m_pStatement = NULL;
    }

    m_objLinker.close();
    (void)Link();
}
