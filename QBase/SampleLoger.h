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

#ifndef Q_SAMPLELOGERER_H_
#define Q_SAMPLELOGERER_H_

#include "Loger.h"
#include "Mutex.h"

class CSampleLoger : public CLoger
{
public:
    CSampleLoger(void);
    ~CSampleLoger(void);

    /*设置打印级别，低于该级别的不输出, 设置LOGLV_NOLOG不输出日志*/
    void setPriority(const LOG_LEVEL emLV);
    /*设置日志文件*/
    void setLogFile(const char *pLogFile);
    /*设置日志文件大小*/
    void setLogMaxSize(const size_t iSize); 

    void Write(const char *pszMsg, const size_t iLens);

    void writeLog(const LOG_LEVEL emInLogLv,
        const char *pFile, const char *pFunction, const int iLine, Q_SOCK &fd,
        const char *pFormat, ...);

    void Open(void);

private:
    std::string getLV(LOG_LEVEL emInLogLv);

private:
    FILE *m_pFile;
    LOG_LEVEL m_emPriorityLV;
    size_t m_uiLogSize;
    std::string m_strFilePath;
    CMutex m_objMutex;
    time_t m_LastCheckTime;
};

#endif//Q_SAMPLELOGERER_H_
