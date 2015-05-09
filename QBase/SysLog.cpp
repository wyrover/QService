
#include "SysLog.h"
#include "QString.h"
#include "QTime.h"
#include "Mutex.h"
#include "File.h"
#include "Exception.h"

CSysLoger::CSysLoger(void) : m_pFile(NULL), m_pMutex(NULL)
{
    std::string strCurPath;
    (void)Q_GetProPath(strCurPath);
    m_pFile = fopen((strCurPath + std::string(QService) + std::string(".syslog")).c_str(), "a");
    if (NULL == m_pFile)
    {
        Q_Printf("%s", "open syslog file error.");

        return;
    }

    try
    {
        m_pMutex = new(std::nothrow) CQMutex();
        if (NULL == m_pMutex)
        {
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);
        }
    }
    catch (CQException &e)
    {
        Q_Printf("get an exception. code %d, message %s", e.getErrorCode(), e.getErrorMsg());

        Q_SafeDelete(m_pMutex);
    }
}

CSysLoger::~CSysLoger(void)
{
    try
    {
        if (NULL != m_pFile)
        {
            fclose(m_pFile);
            m_pFile = NULL;
        }

        Q_SafeDelete(m_pMutex);
    }
    catch(...)
    {

    }
}

std::string CSysLoger::getLV(LOG_LEVEL emInLogLv) const
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

/************************************************************************
* Function name:sysLog
* Description  :记录日志
* IN           :emInLogLv --LOG_LEVEL pFile --文件 pFunction --函数 
                iLine --哪一行 pFormat --格式
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
void CSysLoger::sysLog(const LOG_LEVEL emInLogLv,
    const char *pFile, const char *pFunction, const int iLine,
    const char *pFormat, ...)
{
    if (LOGLV_NOLOG == emInLogLv
        || NULL == m_pFile
        || NULL == m_pMutex)
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

    m_pMutex->Lock();
    fwrite(strMsg.c_str(), 1, strMsg.size(), m_pFile);
    fflush(m_pFile);
    m_pMutex->unLock();
}
