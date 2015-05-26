
#include "Cond.h"
#include "Exception.h"

CCond::CCond(void)
{
    int iRtn = Init();
    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }
}

CCond::~CCond(void)
{
    try
    {
        Destroy();
    }
    catch (...)
    {
    	
    }    
}

int CCond::Init(void)
{
#ifdef Q_OS_WIN
    m_Cond = CreateEvent(NULL, FALSE, FALSE, NULL);
    return (int)(NULL == m_Cond ? Q_Error() : Q_RTN_OK);
#else
    return pthread_cond_init(&m_Cond, (const pthread_condattr_t*)NULL);
#endif
}

void CCond::Destroy(void)
{
#ifdef Q_OS_WIN
    if (NULL != m_Cond)
    {
        (void)CloseHandle(m_Cond);
        m_Cond = NULL;
    }
#else
    (void)pthread_cond_destroy(&m_Cond);
#endif

    return;
}

void CCond::Wait(CQMutex *pMutex)
{
    if (NULL == pMutex)
    {
        Q_EXCEPTION(Q_ERROR_NULLPOINTER, "%s", Q_EXCEPTION_NULLPOINTER);
    }

    int iRtn = Q_RTN_OK;

#ifdef Q_OS_WIN
    pMutex->unLock();
    iRtn = (int)WaitForSingleObject(m_Cond, INFINITE);
    if ((int)WAIT_OBJECT_0 != iRtn)
    {
        iRtn = (int)Q_Error();
    }
    pMutex->Lock();
#else
    iRtn = pthread_cond_wait(&m_Cond, pMutex->getMutex());
#endif

    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }  
}

void CCond::Wait(CQMutex *pMutex, const unsigned int uimSec)
{
    if (NULL == pMutex)
    {
        Q_EXCEPTION(Q_ERROR_NULLPOINTER, "%s", Q_EXCEPTION_NULLPOINTER);
    }

    int iRtn = Q_RTN_OK;

#ifdef Q_OS_WIN
    pMutex->unLock();
    iRtn = (int)WaitForSingleObject(m_Cond, uimSec);
    if (WAIT_TIMEOUT != iRtn
        && (int)WAIT_OBJECT_0 != iRtn)
    {
        iRtn = (int)Q_Error();
    }
    pMutex->Lock();
#else
    struct timespec timeToWait;
    struct timeval now;

    gettimeofday(&now, NULL);

    long seconds = uimSec/1000;
    long nanoseconds = (uimSec - seconds * 1000) * 1000000;
    timeToWait.tv_sec = now.tv_sec + seconds;
    timeToWait.tv_nsec = now.tv_usec*1000 + nanoseconds;

    if (timeToWait.tv_nsec >= 1000000000)
    {
        timeToWait.tv_nsec -= 1000000000;
        timeToWait.tv_sec++;
    }

    iRtn = pthread_cond_timedwait(&m_Cond, pMutex->getMutex(), &timeToWait);
#endif

    if (Q_RTN_OK != iRtn)
    {
#ifdef Q_OS_WIN
        if (WAIT_TIMEOUT != iRtn)        
#else
        if (ETIMEDOUT != iRtn)
#endif
        {
            Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
        } 
    }
}

void CCond::Signal(void)
{
    int iRtn = Q_RTN_OK;

#ifdef Q_OS_WIN
    BOOL bRtn = FALSE;

    bRtn = SetEvent(m_Cond);
    iRtn = (int)(FALSE == bRtn ? Q_Error() : Q_RTN_OK);
#else
    iRtn = pthread_cond_signal(&m_Cond);
#endif
    
    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    } 
}
