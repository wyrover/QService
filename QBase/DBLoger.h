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

#ifndef Q_DBLOGER_H_
#define Q_DBLOGER_H_

#include "Loger.h"
#include "QMySQL.h"
#include "Mutex.h"
#include "TcpParser.h"

//数据库日志
class CDBLoger : public CLoger
{
public:
    CDBLoger(void);
    ~CDBLoger(void);

    //初始化
    bool Init(const char *pszIp, unsigned short &usPort, 
        const char *pszUser, const char *pszPWD, const char *pszDB);

    //写
    void writeDBLog(const Q_SOCK &fd, const char *pszPlayerID, const short sType, 
        const char *pszMsg, const size_t iLens);

public:
    //接口
    void Write(const char *pszMsg, const size_t iLens);
    bool Check(void);

private:
    bool Link(void);
    std::string getTableNam(void) const;
    bool createTable(const std::string &strName);

private:
    CDBStatement *m_pStatement;
    std::string m_strTable;
    CMySQLLink m_objLinker;
    CDBUrl m_objUrl;
    CQMutex m_objMutex;
    CTcpParser m_objTcpParser;        
};

#endif//Q_DBLOGER_H_
