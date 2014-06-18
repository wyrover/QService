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

#ifndef Q_RWLOCK_H_
#define Q_RWLOCK_H_

#include "Macros.h"

#ifdef Q_OS_WIN32

//¶ÁÐ´Ëø,Ê§°ÜÓÐÒì³£Å×³ö
class CRWLock
{
public:
    CRWLock(void);
    ~CRWLock(void);

    /*¶ÁËø*/
    void readLock(void);
    /*Ð´Ëø*/
    void writeLock(void);
    /*½âËø*/
    void unLock(void);

private:
    CRITICAL_SECTION m_Write_Lock;
    CRITICAL_SECTION m_Read_Lock;
    volatile LONG m_lReader_Count;
    volatile bool m_bEnter_Writer;
    enum emReadLockCount
    {
        ReadLock_UnLock = 0,
        ReadLock_Lock = 1,
    };
};

#else

class CRWLock
{
public:
    CRWLock(void);
    ~CRWLock(void);

    void readLock(void);
    void writeLock(void);
    void unLock(void);

private:
    pthread_rwlock_t m_RWLock;
};

#endif

#endif//Q_RWLOCK_H_
