
#include "DataBase.h"
#include "Sqlite/sqlite3.h"

CSqlBuffer::CSqlBuffer(void)
{
    m_pBuf = NULL;
}

CSqlBuffer::~CSqlBuffer(void)
{
    Clear();
}

void CSqlBuffer::Clear(void)
{
    if (NULL != m_pBuf)
    {
        sqlite3_free(m_pBuf);
        m_pBuf = NULL;
    }
}

const char* CSqlBuffer::Format(const char* szFormat, ...)
{
    Clear();

    va_list va;

    va_start(va, szFormat);
    m_pBuf = sqlite3_vmprintf(szFormat, va);
    va_end(va);

    return m_pBuf;
}

void CDBUrl::Format(const char *pszFormat, ...)
{
    va_list va;

    va_start(va, pszFormat);
    m_strUrl = Q_FormatVa(pszFormat, va);
    va_end(va);
}

std::string CDBUrl::getInfo(const char *pszFlag)
{
    if (NULL == pszFlag)
    {
        return "";
    }

    if (0 == strlen(pszFlag))
    {
        return "";
    }

    if (m_strUrl.empty())
    {
        return "";
    }

    std::string::size_type iPos = Q_INIT_NUMBER;
    std::string strVal;
    std::string strKey;
    std::list<std::string> lstTmp;
    std::list<std::string>::iterator itTmp;

    Q_Split(m_strUrl, ";", lstTmp);
    for (itTmp = lstTmp.begin(); lstTmp.end() != itTmp; itTmp++)
    {
        iPos = itTmp->find("=");
        if (std::string::npos != iPos)
        {
            strKey = itTmp->substr(0, iPos);
            if (std::string(pszFlag) == strKey)
            {
                strVal = itTmp->substr(iPos + 1, itTmp->size());
            }
        }
    }

    strVal = Q_Trim(strVal);

    return strVal;
}

std::string CDBUrl::getDB(void)
{
    return getInfo("DB");
}

std::string CDBUrl::getHost(void)
{
    return getInfo("Host");
}

std::string CDBUrl::getUserName(void)
{
    return getInfo("UserName");
}

std::string CDBUrl::getPassWord(void)
{
    return getInfo("PassWord");
}

int CDBUrl::getPort(void)
{
    return atoi(getInfo("Port").c_str());
}
