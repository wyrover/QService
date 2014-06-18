/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 *
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

#include "Exception.h"
#include "QString.h"

CException::CException(const int iErroCode, const char *pszFile, 
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

CException:: ~CException(void)
{
    return;
}

const char* CException::getErrorMsg(void)
{
    return m_strMsg.c_str();
}

int CException::getErrorCode(void)
{
    return m_iErroCode;
}
