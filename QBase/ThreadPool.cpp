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

#include "ThreadPool.h"
#include "Cond.h"
#include "Exception.h"

#define Q_THREADPOOL_CONDTIMEOUT   1000

enum RunStatus
{
    RUN = 0,
    STOP,
};

struct ThreadPool
{
    unsigned int uiThreadNum;//线程数
    CMutex objQueueMutex;//消息队列所
    CMutex objMutex_Thread;//线程接收
    CCond objCond_Thread;
    CMutex objMutex_Main;//主线程接收
    CCond objCond_Main;
    long lStop;//停止标志
    long lIndex;//运行的线程数
    std::queue<CTask *> quHigh;
    std::queue<CTask *> quNormal;
    std::queue<CTask *> quLow;
};

class CThreadPoolTask : public CTask
{
public:
    CThreadPoolTask(ThreadPool *pPool) : m_pPool(NULL)
    {
        m_pPool = pPool;
    };

    ~CThreadPoolTask(void) {};

    void Run(void)
    {
        CTask *pTask = NULL;
        Q_AtomicAdd(&(m_pPool->lIndex), 1);

        m_pPool->objMutex_Main.Lock();
        m_pPool->objCond_Main.Signal();
        m_pPool->objMutex_Main.unLock();

        while(RUN == Q_AtomicGet(&(m_pPool->lStop)))
        {
            m_pPool->objMutex_Thread.Lock();
            m_pPool->objCond_Thread.Wait(&(m_pPool->objMutex_Thread), Q_THREADPOOL_CONDTIMEOUT);
            m_pPool->objMutex_Thread.unLock();

            while(NULL != (pTask = getTask()))
            {
                pTask->Run();
                Q_SafeDelete(pTask);
            }
        }

        Q_AtomicAdd(&(m_pPool->lIndex), -1);

        m_pPool->objMutex_Main.Lock();
        m_pPool->objCond_Main.Signal();
        m_pPool->objMutex_Main.unLock();
    };

private:
    CTask *getTaskFromQueue(std::queue<CTask *> &taskQueue)
    {
        CTask *pTask = NULL;

        m_pPool->objQueueMutex.Lock();
        if (!(taskQueue.empty()))
        {
            pTask = taskQueue.front();
            taskQueue.pop();
        }
        m_pPool->objQueueMutex.unLock();

        return pTask;
    };

    CTask *getTask(void)
    {
        CTask *pTask = NULL;

        pTask = getTaskFromQueue(m_pPool->quHigh);
        if (NULL != pTask)
        {
            return pTask;
        }

        pTask = getTaskFromQueue(m_pPool->quNormal);
        if (NULL != pTask)
        {
            return pTask;
        }

        pTask = getTaskFromQueue(m_pPool->quLow);
        if (NULL != pTask)
        {
            return pTask;
        }

        return NULL;
    }

private:
    ThreadPool *m_pPool;
};

CThreadPool::CThreadPool(const unsigned int uiThreadNum) : m_pstPool(NULL)
{
    int iRtn = Init(uiThreadNum);
    if (Q_RTN_OK != iRtn)
    {
        Destroy();

        Q_EXCEPTION(iRtn, "%s", "init thread pool error.");
    }
}

CThreadPool::~CThreadPool(void)
{
    Destroy();
}

int CThreadPool::Init(const unsigned int &uiThreadNum)
{
    //初始化m_pstPool
    m_pstPool = new(std::nothrow) ThreadPool();
    if (NULL == m_pstPool)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);
        
        return Q_ERROR_ALLOCMEMORY;
    }

    m_pstPool->uiThreadNum = uiThreadNum;
    if (0 == uiThreadNum)
    {
        m_pstPool->uiThreadNum = 1;
    }

    m_pstPool->lIndex = Q_INIT_NUMBER;
    m_pstPool->lStop = RUN;    

    //启动指定数量的线程
    try
    {
        CThread objThread;

        for (unsigned int i = 0; i < m_pstPool->uiThreadNum; i++)
        {
            CThreadPoolTask *pTask = new(std::nothrow) CThreadPoolTask(m_pstPool);
            if (NULL == pTask)
            {
                Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

                return Q_ERROR_ALLOCMEMORY;
            }

            m_pstPool->objMutex_Main.Lock();
            objThread.Execute(pTask);
            m_pstPool->objCond_Main.Wait(&(m_pstPool->objMutex_Main), Q_THREADPOOL_CONDTIMEOUT);
            m_pstPool->objMutex_Main.unLock();
        }
    }
    catch(CException &e)
    {
        Q_Printf("run thread exception, code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());

        return e.getErrorCode();
    }

    return Q_RTN_OK;
}

void CThreadPool::Destroy(void)
{
    if (NULL == m_pstPool)
    {
        return;
    }

    unsigned int uiTimeCount = Q_INIT_NUMBER;
    unsigned int uiTaskNum = Q_INIT_NUMBER;

    /*等待队列中的任务处理完成*/
    while(0 != (uiTaskNum = getTaskNumber()))
    {
        Q_Sleep(500);
        uiTimeCount += 500;

        if (uiTimeCount >= 5000)
        {
            Q_Printf("wait %d task run over.", uiTaskNum);
            uiTimeCount = 0;
        }
    }

    Q_AtomicSet(&(m_pstPool->lStop), STOP);

    m_pstPool->objMutex_Thread.Lock();
    m_pstPool->objCond_Thread.Signal();
    m_pstPool->objMutex_Thread.unLock();

    /*等待所有线程退出*/
    while(Q_AtomicGet(&(m_pstPool->lIndex)) > 0)
    {
        m_pstPool->objMutex_Thread.Lock();
        m_pstPool->objCond_Thread.Signal();
        m_pstPool->objMutex_Thread.unLock();

        m_pstPool->objMutex_Main.Lock();
        m_pstPool->objCond_Main.Wait(&(m_pstPool->objMutex_Main), Q_THREADPOOL_CONDTIMEOUT);
        m_pstPool->objMutex_Main.unLock();
    }

    Q_SafeDelete(m_pstPool);

    return;
}

unsigned int CThreadPool::getTaskNumber(void)
{
    unsigned int uiSize = Q_INIT_NUMBER;

    m_pstPool->objQueueMutex.Lock();
    uiSize += m_pstPool->quHigh.size();
    uiSize += m_pstPool->quNormal.size();
    uiSize += m_pstPool->quLow.size();
    m_pstPool->objQueueMutex.unLock();

    return uiSize;
}

unsigned int CThreadPool::getPoolSize(void)
{
    return m_pstPool->uiThreadNum;
}

/************************************************************************
* Function name:Append
* Description  :向线程池中增加任务,任务指针在执行完成后自动删除
* IN           :pTask --任务 taskLV --级别
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
void CThreadPool::Append(CTask *pTask, TaskLevel taskLV)
{
    if(NULL == pTask)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return;
    }

    m_pstPool->objQueueMutex.Lock();
    switch(taskLV)
    {
    case Q_ThreadLV_High:
        m_pstPool->quHigh.push(pTask);            
        break;

    case Q_ThreadLV_Normal:
        m_pstPool->quNormal.push(pTask);        
        break;

    case Q_ThreadLV_Low:
        m_pstPool->quLow.push(pTask);
        break;

    default:
        break;
    }
    m_pstPool->objQueueMutex.unLock();

    m_pstPool->objMutex_Thread.Lock();
    m_pstPool->objCond_Thread.Signal();
    m_pstPool->objMutex_Thread.unLock();

    return;
}
