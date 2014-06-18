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

#include "DBPool.h"
#include "Thread.h"

enum
{
    NONE = 0,
    RUN,
    STOP,
};

struct stBDMaxPoolInfo
{
    time_t time;
    CDBLink *pLinker;
    stBDMaxPoolInfo(void) : time(0), pLinker(NULL)
    {};
};

class CDBPoolTask : public CTask
{
public:
    CDBPoolTask(void){};
    ~CDBPoolTask(void){};

    void setFreeTime(unsigned int *pFreeTime)
    {
        m_pFreeTime = pFreeTime;
    };
    void setRunFlag(long *pRunFlag)
    {
        m_pRunFlag = pRunFlag;
    };
    void setMutex_Exit(CMutex *pMutex_Exit)
    {
        m_pMutex_Exit = pMutex_Exit;
    };
    void setCond_Exit(CCond *pCond_Exit)
    {
        m_pCond_Exit = pCond_Exit;
    };
    void setMutex_Start(CMutex *pMutex_Start)
    {
        m_pMutex_Start = pMutex_Start;
    };
    void setCond_Start(CCond *pCond_Start)
    {
        m_pCond_Start = pCond_Start;
    };
    void setMaxMutex(CMutex *pMaxMutex)
    {
        m_pMaxMutex = pMaxMutex;
    };
    void setMaxLinker(std::list<struct stBDMaxPoolInfo *> *pMaxLinker)
    {
        m_pMaxLinker = pMaxLinker;
    };

    void Run(void)
    {
        time_t now;
        std::list<struct stBDMaxPoolInfo *>::iterator itMaxPool;

        Q_AtomicSet(m_pRunFlag, (long)RUN);

        m_pMutex_Start->Lock();
        m_pCond_Start->Signal();
        m_pMutex_Start->unLock();

        while(RUN == Q_AtomicGet(m_pRunFlag))
        {
            m_pMaxMutex->Lock(); 
            now = time(NULL);
            for (itMaxPool = m_pMaxLinker->begin(); m_pMaxLinker->end() != itMaxPool;)
            {
                if (((unsigned int)(abs(now - (*itMaxPool)->time)) > (*m_pFreeTime))
                    && (0 != (*itMaxPool)->time)
                    && !((*itMaxPool)->pLinker->getUsed()))
                {
                    Q_SafeDelete((*itMaxPool)->pLinker);
                    Q_SafeDelete((*itMaxPool));

                    itMaxPool = m_pMaxLinker->erase(itMaxPool);

                    continue;
                }

                itMaxPool++;
            }
            m_pMaxMutex->unLock();

            m_pMutex_Start->Lock();
            m_pCond_Start->Wait(m_pMutex_Start, 5000);
            m_pMutex_Start->unLock();
        }

        m_pMutex_Exit->Lock();
        m_pCond_Exit->Signal();
        m_pMutex_Exit->unLock();
    }

private:
    unsigned int *m_pFreeTime;
    long *m_pRunFlag;
    CMutex *m_pMutex_Exit;
    CCond *m_pCond_Exit;
    CMutex *m_pMaxMutex;
    CMutex *m_pMutex_Start;
    CCond *m_pCond_Start;
    std::list<struct stBDMaxPoolInfo *> *m_pMaxLinker;
};


/********************************************/
CDBPool::CDBPool(void)
{
    m_lRunFlag = NONE;
    m_usMaxLinkerNum = 0;
    m_uiFreeTime = 5 * 60;
    m_usMinPoolSize = 10;
    m_usMaxPoolSize = 15;
    m_emType = DBType_MySql;
}

CDBPool::~CDBPool(void)
{
    Stop();
}

void CDBPool::setUrl(CDBUrl &objUrl)
{
    m_objUrl = objUrl;
}

void CDBPool::setMinSize(unsigned short usMinPoolSize)
{
    m_usMinPoolSize = usMinPoolSize;
}

void CDBPool::setMaxSize(unsigned short usMaxPoolSize)
{
    m_usMaxPoolSize = usMaxPoolSize;
}

void CDBPool::setType(DBType emType)
{
    m_emType = emType;
}

void CDBPool::setMaxFreeTime(unsigned int usMs)
{
    m_uiFreeTime = usMs;
}

int CDBPool::getLinker(CDBLink **ppLinker)
{
    switch(m_emType)
    {
    case DBType_Sqlite3:
        *ppLinker = new(std::nothrow) CSQLite3Link();
        break;

    case DBType_MySql:
        *ppLinker = new(std::nothrow) CMySQLLink();
        break;

    default:
        Q_Printf("%s", "unkown database type");
        break;
    }

    if (NULL == *ppLinker)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

        return Q_ERROR_ALLOCMEMORY;
    }

    try
    {
        (*ppLinker)->open(m_objUrl);

        return Q_RTN_OK;
    }
    catch (CException &e)
    {
        Q_SafeDelete(*ppLinker);

        Q_Printf("excption code %d  message %s", e.getErrorCode(), e.getErrorMsg());

        return e.getErrorCode();
    }
}

int CDBPool::Start(void)
{
    int iRtn = Q_RTN_OK;
    CThread objThread;

    if (m_usMaxPoolSize < m_usMinPoolSize)
    {
        Q_Printf("%s", "max size must big than min size.");

        return Q_RTN_FAILE;
    }

    /*初始化最小连接*/
    for (unsigned short us = 0; us < m_usMinPoolSize; us++)
    {
        CDBLink *pLinker = NULL;

        iRtn = getLinker(&pLinker);
        if (Q_RTN_OK != iRtn)
        {
            return iRtn;
        }

        pLinker->setUsed(false);

        m_lstMinLinker.push_back(pLinker);
    }

    m_usMaxLinkerNum = m_usMaxPoolSize - m_usMinPoolSize;
    if (0 == m_usMaxLinkerNum)
    {
        return Q_RTN_OK;
    }

    /*启动超过最小连接数的连接的监控*/
    Q_Printf("%s", "init linker monitor.");
    CDBPoolTask *pPoolTask = new(std::nothrow) CDBPoolTask();
    if (NULL == pPoolTask)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

        return Q_ERROR_ALLOCMEMORY;
    }

    pPoolTask->setCond_Exit(&m_objCond_Exit);
    pPoolTask->setFreeTime(&m_uiFreeTime);
    pPoolTask->setMaxLinker(&m_lstMaxLinker);
    pPoolTask->setMaxMutex(&m_objMaxMutex);
    pPoolTask->setMutex_Exit(&m_objMutex_Exit);
    pPoolTask->setRunFlag(&m_lRunFlag);
    pPoolTask->setCond_Start(&m_objCond_Start);
    pPoolTask->setMutex_Start(&m_objMutex_Start);

    //等待线程启动
    m_objMutex_Start.Lock();
    objThread.Execute(pPoolTask);
    m_objCond_Start.Wait(&m_objMutex_Start, 1000);
    m_objMutex_Start.unLock();

    Q_Printf("%s", "start database pool successfully.");

    return Q_RTN_OK;
}

void CDBPool::Stop(void)
{
    Q_Printf("%s", "stop database pool.");

    if (RUN == Q_AtomicGet(&m_lRunFlag))
    {
        /*等待监控线程退出*/
        Q_Printf("%s", "wait monitor exit.");

        m_objMutex_Exit.Lock();
        Q_AtomicSet(&m_lRunFlag, (long)STOP);
        m_objMutex_Start.Lock();
        m_objCond_Start.Signal();
        m_objMutex_Start.unLock();
        m_objCond_Exit.Wait(&m_objMutex_Exit, 1000);
        m_objMutex_Exit.unLock();
    }

    int iCount = Q_INIT_NUMBER;
    unsigned int uiTime = Q_INIT_NUMBER;
    std::list<CDBLink *>::iterator itLinker;
    std::list<struct stBDMaxPoolInfo *>::iterator itMaxLinker;

    Q_Printf("%s", "wait all db linker free....");
    /*等待所有连接释放*/
    while((!m_lstMinLinker.empty())
        || (!m_lstMaxLinker.empty()))
    {
        iCount = 0;

        for (itLinker = m_lstMinLinker.begin(); m_lstMinLinker.end() != itLinker; )
        {
            if (!((*itLinker)->getUsed()))
            {
                Q_SafeDelete(*itLinker);
                itLinker = m_lstMinLinker.erase(itLinker);

                continue;
            }

            iCount++;
            itLinker++;
        }

        for (itMaxLinker = m_lstMaxLinker.begin(); m_lstMaxLinker.end() != itMaxLinker;)
        {
            if (!((*itMaxLinker)->pLinker->getUsed()))
            {
                Q_SafeDelete((*itMaxLinker)->pLinker);
                Q_SafeDelete((*itMaxLinker));
                itMaxLinker = m_lstMaxLinker.erase(itMaxLinker);

                continue;
            }

            iCount++;
            itMaxLinker++;
        }

        Q_Sleep(100);
        uiTime += 100;

        if (uiTime >= 5000)
        {
            Q_Printf("wait %d db linker free.", iCount);
            uiTime = 0;
        }
    }
}

CDBLink *CDBPool::getFromMin(void)
{
    int iRtn = Q_RTN_OK;
    CDBLink *pLinker = NULL;
    std::list<CDBLink *>::iterator itLinker;

    m_objMinMutex.Lock();
    for (itLinker = m_lstMinLinker.begin(); m_lstMinLinker.end() != itLinker; itLinker++)
    {
        if (!((*itLinker)->getUsed()))
        {
            pLinker = *itLinker;
            iRtn = pLinker->reLink();
            if (Q_RTN_OK != iRtn)
            {
                try
                {
                    pLinker->close();
                    pLinker->open(m_objUrl);
                    pLinker->setUsed(true);

                    break;
                }
                catch (CException &e)
                {
                    pLinker = NULL;
                    Q_Printf("excption code %d  message %s", e.getErrorCode(), e.getErrorMsg());
                }
            }
            else
            {
                pLinker->setUsed(true);

                break;
            }
        }
    }
    m_objMinMutex.unLock();

    return pLinker;
}

CDBLink *CDBPool::getFromMax(void)
{
    int iRtn = Q_RTN_OK;
    CDBLink *pLinker = NULL;
    std::list<struct stBDMaxPoolInfo *>::iterator itMaxPool;

    m_objMaxMutex.Lock();
    for (itMaxPool = m_lstMaxLinker.begin(); m_lstMaxLinker.end() != itMaxPool; itMaxPool++)
    {
        if (!((*itMaxPool)->pLinker->getUsed()))
        {
            pLinker = (*itMaxPool)->pLinker;
            iRtn = pLinker->reLink();
            if (Q_RTN_OK != iRtn)
            {
                try
                {
                    pLinker->close();
                    pLinker->open(m_objUrl);
                    pLinker->setUsed(true);
                    (*itMaxPool)->time = 0;

                    break;
                }
                catch (CException &e)
                {
                    pLinker = NULL;
                    Q_Printf("excption code %d  message %s", e.getErrorCode(), e.getErrorMsg());
                }
            }
            else
            {
                pLinker->setUsed(true);
                (*itMaxPool)->time = 0;

                break;
            }
        }
    }

    if (NULL == pLinker)
    {
        if (m_lstMaxLinker.size() < m_usMaxLinkerNum)
        {
            if (Q_RTN_OK == getLinker(&pLinker))
            {
                stBDMaxPoolInfo *pInfo = new(std::nothrow) stBDMaxPoolInfo();
                if (NULL != pInfo)
                {
                    pInfo->pLinker = pLinker;
                    pInfo->time = 0;
                    pInfo->pLinker->setUsed(true);

                    m_lstMaxLinker.push_back(pInfo);
                }
                else
                {
                    Q_SafeDelete(pLinker);

                    Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);
                }
            }
        }
    }

    m_objMaxMutex.unLock();

    return pLinker;
}

CDBLink *CDBPool::mallocLinker(void)
{
    CDBLink *pLinker = NULL;

    pLinker = getFromMin();
    if (NULL != pLinker)
    {
        return pLinker;
    }

    if (0 == m_usMaxLinkerNum)
    {
        return NULL;
    }

    return getFromMax();
}

void CDBPool::freeLinker(CDBLink *pLinker)
{
    std::list<CDBLink *>::iterator itLinker;
    std::list<struct stBDMaxPoolInfo *>::iterator itMaxPool;

    m_objMinMutex.Lock();
    for (itLinker = m_lstMinLinker.begin(); m_lstMinLinker.end() != itLinker; itLinker++)
    {
        if (pLinker == (*itLinker))
        {
            (*itLinker)->setUsed(false);
            m_objMinMutex.unLock();

            return;
        }
    }
    m_objMinMutex.unLock();

    m_objMaxMutex.Lock();
    for (itMaxPool = m_lstMaxLinker.begin(); m_lstMaxLinker.end() != itMaxPool; itMaxPool++)
    {
        if (pLinker == (*itMaxPool)->pLinker)
        {
            pLinker->setUsed(false);
            (*itMaxPool)->time = time(NULL);
            m_objMaxMutex.unLock();

            return;
        }
    }
    m_objMaxMutex.unLock();
}
