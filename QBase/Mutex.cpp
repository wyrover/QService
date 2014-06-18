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

#include "Mutex.h"
#include "Exception.h"

CMutex::CMutex(void)
{
    int iRtn = Init();
    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }
}

CMutex::~CMutex(void)
{
    Destroy();
}

int CMutex::Init(void)
{
#ifdef Q_OS_WIN32
    m_Mutex = CreateMutex(NULL, FALSE, NULL);
    return (NULL == m_Mutex ? Q_Error() : Q_RTN_OK);
#else
    return pthread_mutex_init(&m_Mutex, (const pthread_mutexattr_t*)NULL);
#endif
}

void CMutex::Destroy(void)
{
#ifdef Q_OS_WIN32
    if (NULL != m_Mutex)
    {
        (void)CloseHandle(m_Mutex);
        m_Mutex = NULL;
    }
#else
    (void)pthread_mutex_destroy(&m_Mutex);
#endif

    return;
}

void CMutex::Lock(void)
{
    int iRtn = Q_RTN_OK;

#ifdef Q_OS_WIN32
    iRtn = WaitForSingleObject(m_Mutex, INFINITE);
    iRtn = (WAIT_OBJECT_0 == iRtn ? Q_RTN_OK : Q_Error());
#else
    iRtn = pthread_mutex_lock(&m_Mutex);
#endif

    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }
}

void CMutex::unLock(void)
{
    int iRtn = Q_RTN_OK;

#ifdef Q_OS_WIN32
    BOOL bRtn = FALSE;

    bRtn = ReleaseMutex(m_Mutex);
    iRtn = (FALSE == bRtn ? Q_Error() : Q_RTN_OK);
#else
    iRtn = pthread_mutex_unlock(&m_Mutex);
#endif

    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }
}

Q_Mutex_t *CMutex::getMutex(void)
{
    return &m_Mutex;
}
