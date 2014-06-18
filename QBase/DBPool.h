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
#ifndef Q_DBPOOL_H_
#define Q_DBPOOL_H_

#include "QMySQL.h"
#include "Sqlite.h"
#include "Cond.h"

/*数据库连接池*/
class CDBPool
{
public:
    CDBPool(void);
    ~CDBPool(void);

    void setUrl(CDBUrl &objUrl);
    /*数据库连接池最小连接数*/
    void setMinSize(unsigned short usMinPoolSize);
    /*数据库连接池最大连接数*/
    void setMaxSize(unsigned short usMaxPoolSize);
    void setType(DBType emType);
    /*空闲时间(秒)， 超时将回收未使用的额外连接*/
    void setMaxFreeTime(unsigned int usS);

    int Start(void);
    void Stop(void);

    CDBLink *mallocLinker(void);
    void freeLinker(CDBLink *pLinker);

private:
    int getLinker(CDBLink **ppLinker);
    CDBLink *getFromMin(void);
    CDBLink *getFromMax(void);

private:
    long m_lRunFlag;
    CDBUrl m_objUrl;
    unsigned short m_usMinPoolSize;
    unsigned short m_usMaxPoolSize;
    unsigned short m_usMaxLinkerNum;
    DBType m_emType;
    unsigned int m_uiFreeTime;
    std::list<CDBLink *> m_lstMinLinker;
    std::list<struct stBDMaxPoolInfo *> m_lstMaxLinker;
    CMutex m_objMinMutex;
    CMutex m_objMaxMutex;
    CMutex m_objMutex_Exit;
    CCond m_objCond_Exit;
    CMutex m_objMutex_Start;
    CCond m_objCond_Start;
};

#endif//Q_DBPOOL_H_
