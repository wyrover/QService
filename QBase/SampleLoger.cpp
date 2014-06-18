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

#include "SampleLoger.h"
#include "InitSock.h"
#include "QTime.h"
#include "QString.h"
#include "File.h"

CSampleLoger::CSampleLoger(void)
{
    m_uiLogSize = 5*1024*1024;
    m_pFile = NULL;
    m_emPriorityLV = LOGLV_INFO;
    m_LastCheckTime = time(NULL);
}

CSampleLoger::~CSampleLoger(void)
{
    if (NULL != m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }
}

void CSampleLoger::setPriority(const LOG_LEVEL emLV)
{
    m_emPriorityLV = emLV;
}

void CSampleLoger::setLogFile(const char *pLogFile)
{
    m_strFilePath = pLogFile;
}

void CSampleLoger::setLogMaxSize(const size_t iSize)
{
    m_uiLogSize = iSize;
}

void CSampleLoger::Open(void)
{
    m_pFile = fopen(m_strFilePath.c_str(), "a");
    if(NULL == m_pFile)
    {
        Q_Printf("open file %s error.", m_strFilePath.c_str());

        return;
    }

    return;
}

std::string CSampleLoger::getLV(LOG_LEVEL emInLogLv)
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

void CSampleLoger::Write(const char *pszMsg, const size_t iLens)
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
            strTmpPath = Q_Replace(strTmpPath, std::string("-"), std::string(""));
            strTmpPath = Q_Replace(strTmpPath, std::string(":"), std::string(""));
            strTmpPath = Q_Replace(strTmpPath, std::string(" "), std::string(""));

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

void CSampleLoger::writeLog(const LOG_LEVEL emInLogLv,
    const char *pFile, const char *pFunction, const int iLine, Q_SOCK &fd,
    const char *pFormat, ...)
{
    if (emInLogLv > m_emPriorityLV)
    {
        return;
    }

    if (NULL == pFormat
        || NULL == pFunction
        || NULL == pFile
        || Q_INVALID_SOCK == fd)
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

    m_objMutex.Lock();
    (void)Q_SockWrite(fd, strMsg.c_str(), strMsg.size());
    m_objMutex.unLock();

    return;
}
