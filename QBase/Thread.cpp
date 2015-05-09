
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
    try
    {
        Destroy();
    }
    catch(...)
    {

    }    
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

int CThread::attrInit(Q_Thread_Attr_t *pAttr) const
{
#ifdef Q_OS_WIN32
    *pAttr = 0;

    return Q_RTN_OK;
#else
    return pthread_attr_init(pAttr);
#endif
}

int CThread::attrDestroy(Q_Thread_Attr_t *pAttr) const
{
#ifdef Q_OS_WIN32
    *pAttr = 0;

    return Q_RTN_OK;
#else
    return pthread_attr_destroy(pAttr);
#endif
}

int CThread::attrSetDetaChstate(Q_Thread_Attr_t *pAttr, int iDetachstate) const
{
#ifdef Q_OS_WIN32
    *pAttr |= iDetachstate;

    return Q_RTN_OK;
#else
    return pthread_attr_setdetachstate(pAttr, iDetachstate);
#endif
}

int CThread::Create(Q_Thread_t *pThread, Q_Thread_Attr_t *pAttr,
                                Q_Thread_Func_t myfunc, void *args) const
{
#ifdef Q_OS_WIN32
    HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, myfunc, args, 0, pThread);
    return (int)((NULL ==handle) ? Q_Error() : Q_RTN_OK);
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
