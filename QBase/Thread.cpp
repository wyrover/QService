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

#include "Thread.h"
#include "Exception.h"

CThread::CThread(void)
{
    int iRtn = Init();
    if (Q_RTN_OK != iRtn)
    {
        Destroy();

        Q_EXCEPTION(iRtn, "%s", "init thread error.");
    }
}

CThread::~CThread(void)
{
    Destroy();
}

int CThread::Init(void)
{
    int iRtn = Q_RTN_OK;

    iRtn = attrInit(&m_Attr);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    iRtn = attrSetDetaChstate(&m_Attr, Q_THREAD_CREATE_DETACHED);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    return Q_RTN_OK;
}

void CThread::Destroy(void)
{
    (void)attrDestroy(&m_Attr);
}

int CThread::attrInit(Q_Thread_Attr_t *pAttr)
{
#ifdef Q_OS_WIN32
    *pAttr = 0;

    return Q_RTN_OK;
#else
    return pthread_attr_init(pAttr);
#endif
}

int CThread::attrDestroy(Q_Thread_Attr_t *pAttr)
{
#ifdef Q_OS_WIN32
    *pAttr = 0;

    return Q_RTN_OK;
#else
    return pthread_attr_destroy(pAttr);
#endif
}

int CThread::attrSetDetaChstate(Q_Thread_Attr_t *pAttr, int iDetachstate)
{
#ifdef Q_OS_WIN32
    *pAttr |= iDetachstate;

    return Q_RTN_OK;
#else
    return pthread_attr_setdetachstate(pAttr, iDetachstate);
#endif
}

int CThread::Create(Q_Thread_t *pThread, Q_Thread_Attr_t *pAttr,
                                Q_Thread_Func_t myfunc, void *args)
{
#ifdef Q_OS_WIN32
    HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, myfunc, args, 0, pThread);
    return (handle > 0 ? Q_RTN_OK : Q_Error());
#else
    if (NULL == pAttr)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return Q_ERROR_NULLPOINTER;
    }

    return pthread_create(pThread, pAttr, myfunc, args);
#endif
}

Q_Thread_Result_t Q_THREAD_CALL Run(void *args)
{
    CTask *pTask = (CTask*)args;

    try
    {
        pTask->Run();        
    }
    catch(CQException &e)
    {
        Q_Printf("run task exception. error code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
    }
    catch(...)
    {
        Q_Printf("%s", "run task get an exception.");
    }

    Q_SafeDelete(pTask);

    return NULL;
}

void CThread::Execute(CTask *pTask, Q_Thread_t *pThread)
{
    if (NULL == pThread)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return;
    }

    if (NULL == pTask)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return;
    }

    int iRtn = Q_RTN_OK;

    iRtn = Create(pThread, &m_Attr, Run, pTask);
    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }
}

/************************************************************************
* Function name:Execute
* Description  :开启一线程执行任务,任务指针在执行完成后自动删除(失败抛出异常)
* IN           :pTask --任务
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
void CThread::Execute(CTask *pTask)
{
    Q_Thread_t Thread;

    Execute(pTask, &Thread);
}

/************************************************************************
* Function name:Q_Sleep
* Description  :休眠(ms)
* IN           :iMsec --时间(ms)
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
void Q_Sleep(const unsigned int iMsec)
{
#ifdef Q_OS_WIN32
    Sleep(iMsec);
#else
    pthread_mutex_t fakeMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t fakeCond = PTHREAD_COND_INITIALIZER;
    struct timespec timeToWait;
    struct timeval now;

    gettimeofday(&now, NULL);

    long seconds = iMsec/1000;
    long nanoseconds = (iMsec - seconds * 1000) * 1000000;
    timeToWait.tv_sec = now.tv_sec + seconds;
    timeToWait.tv_nsec = now.tv_usec*1000 + nanoseconds;

    if (timeToWait.tv_nsec >= 1000000000)
    {
        timeToWait.tv_nsec -= 1000000000;
        timeToWait.tv_sec++;
    }

    pthread_mutex_lock(&fakeMutex);
    (void)pthread_cond_timedwait(&fakeCond, &fakeMutex, &timeToWait);
    pthread_mutex_unlock(&fakeMutex);
#endif

    return;
}

Q_Thread_t Q_ThreadId(void)
{
#ifdef Q_OS_WIN32
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
}
