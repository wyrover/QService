
#include "TxtLoger.h"
#include "InitSock.h"
#include "QTime.h"
#include "QString.h"
#include "File.h"

SINGLETON_INIT(CTxtLoger)
CTxtLoger objTxtLoger;

CTxtLoger::CTxtLoger(void)
{
    m_uiLogSize = 5*1024*1024;
    m_pFile = NULL;
    m_emPriorityLV = LOGLV_INFO;
    m_LastCheckTime = time(NULL);
}

CTxtLoger::~CTxtLoger(void)
{
    if (NULL != m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }
}

void CTxtLoger::setPriority(const LOG_LEVEL emLV)
{
    m_emPriorityLV = emLV;
}

void CTxtLoger::setLogFile(const char *pLogFile)
{
    m_strFilePath = pLogFile;
}

void CTxtLoger::setLogMaxSize(const size_t iSize)
{
    m_uiLogSize = iSize;
}

void CTxtLoger::Open(void)
{
    m_pFile = fopen(m_strFilePath.c_str(), "a");
    if(NULL == m_pFile)
    {
        Q_Printf("open file %s error.", m_strFilePath.c_str());

        return;
    }

    return;
}

std::string CTxtLoger::getLV(LOG_LEVEL emInLogLv) const
{
    std::string strRst;

    switch(emInLogLv)
    {
    case LOGLV_WARN:
        strRst = "WARNING";
        break;

    case LOGLV_ERROR:
        strRst = "ERROR";
        break;

    case LOGLV_INFO:
        strRst = "INFO";
        break;

    case LOGLV_DEBUG:
        strRst = "DEBUG";
        break;

    default:
        strRst = "OTHER";
        break;
    }

    return strRst;
}

void CTxtLoger::Write(const char *pszMsg, const size_t iLens)
{
    if (NULL == m_pFile)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return;
    }

    time_t now = time(NULL);

    if ((int)(now - m_LastCheckTime) > 5 * 60)
    {
        m_LastCheckTime = now;

        unsigned long ulSize = Q_INIT_NUMBER;

        (void)Q_FileSize(m_strFilePath.c_str(), ulSize);
        if (ulSize > m_uiLogSize)
        {
            std::string strTmpPath;

            strTmpPath = Q_Now();
            strTmpPath = Q_Replace(strTmpPath, "-", "");
            strTmpPath = Q_Replace(strTmpPath, ":", "");
            strTmpPath = Q_Replace(strTmpPath, " ", "");

            strTmpPath = m_strFilePath + std::string(".") + strTmpPath;

            fclose(m_pFile);
            m_pFile = NULL;

            (void)Q_FileReName(m_strFilePath.c_str(), strTmpPath.c_str());
            m_pFile = fopen(m_strFilePath.c_str(), "a");        
            if (NULL == m_pFile)
            {
                Q_Printf("open file %s error.", m_strFilePath.c_str());

                return;
            }
        }
    }

    fwrite(pszMsg, 1, iLens, m_pFile);
    if (m_emPriorityLV >= LOGLV_DEBUG)
    {
        fflush(m_pFile);
    }
}

void CTxtLoger::writeLog(const LOG_LEVEL emInLogLv,
    const char *pFile, const char *pFunction, const int iLine,
    const char *pFormat, ...)
{
    if (emInLogLv > m_emPriorityLV)
    {
        return;
    }

    if (NULL == pFormat
        || NULL == pFunction
        || NULL == pFile
        || Q_INVALID_SOCK == getSock())
    {
        return;
    }

    std::string strVa;
    std::string strMsg;
    std::string strFile;
    va_list va;

    strFile = std::string(pFile);
    strFile = Q_GetLastOfFlag(strFile, Q_PATH_SEPARATOR);

    va_start(va, pFormat);
    strVa = Q_FormatVa(pFormat, va);
    va_end(va);

    strMsg = Q_FormatStr("[%s][%s %s %d][%s] %s\n", 
        Q_Now().c_str(), 
        strFile.c_str(), pFunction, iLine,
        getLV(emInLogLv).c_str(), 
        strVa.c_str());

    size_t iHeadLens = Q_INIT_NUMBER;
    const char *pszHead = m_objTcpParser.createHead(strMsg.size(), iHeadLens);

    m_objMutex.Lock();
    (void)Q_SockWrite(getSock(), pszHead, iHeadLens);
    (void)Q_SockWrite(getSock(), strMsg.c_str(), strMsg.size());
    m_objMutex.unLock();

    return;
}
