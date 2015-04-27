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

#include "SysLog.h"
#include "QString.h"
#include "QTime.h"
#include "Mutex.h"
#include "File.h"
#include "Exception.h"

CSysLoger::CSysLoger(void) : m_pFile(NULL), m_pMutex(NULL)
{
    std::string strCurPath;
    Q_GetProPath(strCurPath);
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
    if (NULL != m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }

    Q_SafeDelete(m_pMutex);
}

std::string CSysLoger::getLV(LOG_LEVEL emInLogLv)
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
