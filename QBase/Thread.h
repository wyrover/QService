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

#ifndef Q_THREAD_H_
#define Q_THREAD_H_

#include "Macros.h"
#include "CTask.h"

/*线程类*/
class CThread
{
public:
    CThread(void);
    ~CThread(void);

    /*启动一线程执行任务,任务指针在执行完成后自动删除*/
    void Execute(CTask *pTask);

    /*启动一线程执行任务,任务指针在执行完成后自动删除*/
    void Execute(CTask *pTask, Q_Thread_t *pThread);    

private:
    int attrInit(Q_Thread_Attr_t *pAttr);
    int attrDestroy(Q_Thread_Attr_t *pAttr);
    int attrSetDetaChstate(Q_Thread_Attr_t *pAttr, int iDetachstate);
    int Create(Q_Thread_t *pThread, Q_Thread_Attr_t *pAttr,
        Q_Thread_Func_t myfunc, void *args);
    int Init(void);
    void Destroy(void);

private:
    Q_Thread_Attr_t m_Attr;
};

/*获取线程ID*/
Q_Thread_t Q_ThreadId(void);
/*休眠(ms)*/
void Q_Sleep(const unsigned int iMsec);

#endif//Q_THREAD_H_
