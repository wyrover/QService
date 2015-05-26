
#include "Mutex.h"
#include "Exception.h"

CQMutex::CQMutex(void)
{
    int iRtn = Init();
    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }
}

CQMutex::~CQMutex(void)
{
    try
    {
        Destroy();
    }
    catch(...)
    {

    }    
}

int CQMutex::Init(void)
{
#ifdef Q_OS_WIN
    InitializeCriticalSection(&m_Mutex);
    return Q_RTN_OK;
#else
    return pthread_mutex_init(&m_Mutex, (const pthread_mutexattr_t*)NULL);
#endif
}

void CQMutex::Destroy(void)
{
#ifdef Q_OS_WIN
    DeleteCriticalSection(&m_Mutex);
#else
    (void)pthread_mutex_destroy(&m_Mutex);
#endif

    return;
}

void CQMutex::Lock(void)
{
    int iRtn = Q_RTN_OK;

#ifdef Q_OS_WIN
    EnterCriticalSection(&m_Mutex);
#else
    iRtn = pthread_mutex_lock(&m_Mutex);
#endif

    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }
}

void CQMutex::unLock(void)
{
    int iRtn = Q_RTN_OK;

#ifdef Q_OS_WIN
    LeaveCriticalSection(&m_Mutex);
#else
    iRtn = pthread_mutex_unlock(&m_Mutex);
#endif

    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }
}

Q_Mutex_t *CQMutex::getMutex(void)
{
    return &m_Mutex;
}
