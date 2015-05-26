
#include "Charset.h"
#include <uchardet/uchardet.h>

CCharset::CCharset(void) : m_pCharset(NULL)
{    
    m_pCharset = uchardet_new(); 
}

CCharset::~CCharset(void)
{
    if (NULL != m_pCharset)
    {
        uchardet_delete(m_pCharset);
        m_pCharset = NULL;
    }
}

std::string CCharset::getStrCharset(const char *pszData, const unsigned int iSize)
{
    if (NULL == pszData
        || NULL == m_pCharset)
    {
        return "";
    }

    uchardet_reset(m_pCharset);

    if (Q_RTN_OK != uchardet_handle_data(m_pCharset, pszData, iSize))
    {
        return "";
    }

    uchardet_data_end(m_pCharset);

    return std::string(uchardet_get_charset(m_pCharset));
}

std::string CCharset::getCharset(FILE *pFile)
{
    if (NULL == pFile
        || NULL == m_pCharset)
    {
        return "";
    }

    size_t iReadSize = 0;
    char acTmp[Q_ONEK];

    uchardet_reset(m_pCharset);

    while(!feof(pFile))
    {
        iReadSize = fread(acTmp, 1, sizeof(acTmp), pFile);
        if (Q_RTN_OK != uchardet_handle_data(m_pCharset, acTmp, iReadSize))
        {
            return "";
        }
    }

    uchardet_data_end(m_pCharset);

    return std::string(uchardet_get_charset(m_pCharset));
}

std::string CCharset::getCharset(const char *pszFile)
{
    if (NULL == pszFile
        || NULL == m_pCharset)
    {
        return "";
    }
    
    FILE *pFile = NULL;

    pFile = fopen(pszFile, "r");
    if (NULL == pFile)
    {
        return "";
    }
   
    std::string strCharset(getCharset(pFile));

    fclose(pFile);

    return strCharset;
}
