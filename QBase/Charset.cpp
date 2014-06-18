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

#include "Charset.h"
#include "uchardet/uchardet.h"

CCharset::CCharset(void) : m_pCharset(NULL)
{
    try
    {
        m_pCharset = uchardet_new(); 
    }
    catch (std::bad_alloc &)
    {
        m_pCharset = NULL;

        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);
    }
}

CCharset::~CCharset(void)
{
    if (NULL != m_pCharset)
    {
        uchardet_delete(m_pCharset);
        m_pCharset = NULL;
    }
}

std::string CCharset::getCharset(const char *pszData, const size_t iSize)
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
