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

#include "RWLock.h"

#ifdef Q_OS_WIN32

CRWLock::CRWLock(void)
{
    m_lReader_Count = 0; 
    m_bEnter_Writer = false;

    //初始化临界区
    InitializeCriticalSection(&m_Write_Lock);
    InitializeCriticalSection(&m_Read_Lock);
}

CRWLock::~CRWLock(void)
{
    //删除临界区
    DeleteCriticalSection(&m_Write_Lock);
    DeleteCriticalSection(&m_Read_Lock);
}

void CRWLock::readLock(void)
{
    //进入临界区
    EnterCriticalSection(&m_Write_Lock);
    if(ReadLock_Lock == ::InterlockedIncrement(&m_lReader_Count))//原子自加 ,如果读锁未锁定，则锁定临界区
    {
        EnterCriticalSection(&m_Read_Lock);
    }

    //离开临界区
    LeaveCriticalSection(&m_Write_Lock);
}

void CRWLock::writeLock(void)
{
    EnterCriticalSection(&m_Write_Lock);
    EnterCriticalSection(&m_Read_Lock);

    m_bEnter_Writer = true;
}

void CRWLock::unLock(void)
{
    if(m_bEnter_Writer)
    {
        m_bEnter_Writer = false;

        LeaveCriticalSection(&m_Read_Lock);
        LeaveCriticalSection(&m_Write_Lock);
    }
    else
    {
        if(ReadLock_UnLock == ::InterlockedDecrement(&m_lReader_Count))//原子自减,所有读锁离开后解锁临界区
        {
            LeaveCriticalSection(&m_Read_Lock);
        }
    }
}

#else

#include "Exception.h"

CRWLock::CRWLock(void)
{
    int iRtn = Q_RTN_OK;

    iRtn = pthread_rwlock_init(&m_RWLock, NULL);
    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }
}

CRWLock::~CRWLock(void)
{
    (void)pthread_rwlock_destroy(&m_RWLock);
}

void CRWLock::readLock(void)
{
    int iRtn = Q_RTN_OK;

    iRtn = pthread_rwlock_rdlock(&m_RWLock);
    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }
}

void CRWLock::writeLock(void)
{
    int iRtn = Q_RTN_OK;

    iRtn = pthread_rwlock_wrlock(&m_RWLock);
    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }
}

void CRWLock::unLock(void)
{
    int iRtn = Q_RTN_OK;

    iRtn = pthread_rwlock_unlock(&m_RWLock);
    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", Q_Error2Str(iRtn));
    }
}

#endif
