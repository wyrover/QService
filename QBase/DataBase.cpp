/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 * github:https://github.com/QService/QService
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

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

    Q_Split(m_strUrl, std::string(";"), lstTmp);
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
