
#include "Exception.h"
#include "QString.h"

CQException::CQException(const int iErroCode, const char *pszFile, 
    const char *pszFunc, const int iLine, const char *pcFormat, ...)
{
    std::string strFunc;
    std::string strFile;
    std::string strVa;
    va_list va;

    va_start(va, pcFormat);
    strVa = Q_FormatVa(pcFormat, va);
    va_end(va);

    if (NULL != pszFunc)
    {
        strFunc = std::string(pszFunc);
    }

    if (NULL != pszFile)
    {
        strFile = std::string(pszFile);
        strFile = Q_GetLastOfFlag(strFile, Q_PATH_SEPARATOR);
    }

    m_iErroCode = iErroCode;

    char acTmp[30] = {0};
    Q_Snprintf(acTmp, sizeof(acTmp) - 1, "%d", iLine);
    m_strMsg = std::string("[") + strFile + 
        std::string(" ") + strFunc + 
        std::string(" ") + std::string(acTmp) + std::string("] ") +
        strVa;

    return;
}

CQException:: ~CQException(void)
{
    return;
}

const char* CQException::getErrorMsg(void)
{
    return m_strMsg.c_str();
}

int CQException::getErrorCode(void)
{
    return m_iErroCode;
}
