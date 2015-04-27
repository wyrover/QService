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
    Destroy();
}

int CCond::Init(void)
{
#ifdef Q_OS_WIN32
    m_Cond = CreateEvent(NULL, FALSE, FALSE, NULL);
    return (NULL == m_Cond ? Q_Error() : Q_RTN_OK);
#else
    return pthread_cond_init(&m_Cond, (const pthread_condattr_t*)NULL);
#endif
}

void CCond::Destroy(void)
{
#ifdef Q_OS_WIN32
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

#ifdef Q_OS_WIN32
    pMutex->unLock();
    iRtn = WaitForSingleObject(m_Cond, INFINITE);
    if (WAIT_OBJECT_0 != iRtn)
    {
        iRtn = Q_Error();
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

#ifdef Q_OS_WIN32
    pMutex->unLock();
    iRtn = WaitForSingleObject(m_Cond, uimSec);
    if (WAIT_TIMEOUT != iRtn
        && WAIT_OBJECT_0 != iRtn)
    {
        iRtn = Q_Error();
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
#ifdef Q_OS_WIN32
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

#ifdef Q_OS_WIN32
    BOOL bRtn = FALSE;

    bRtn = SetEvent(m_Cond);
    iRtn = (FALSE == bRtn ? Q_Error() : Q_RTN_OK);
#else
    iRtn = pthread_cond_signal(&m_Cond);
#endif
    
    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    } 
}
