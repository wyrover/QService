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

#ifndef Q_SOCKPAIREVENT_H_
#define Q_SOCKPAIREVENT_H_

#include "SockPair.h"
#include "EventBuffer.h"
#include "Cond.h"

/*
SockPair事件循环
*/
class CSockPairEvent
{
public:
    CSockPairEvent(void);
    virtual ~CSockPairEvent(void);

    /*启动事件循环*/
    int Start(void);
    /*停止事件循环*/
    void Stop(void);
    /*等待进入事件循环*/
    bool waitForStarted(void);
    /*状态设置*/
    void setRunStatus(RunStatus emStatus);
    RunStatus getRunStatus(void);
    /*onStop是否执行*/
    void setRunOnStop(bool bRun);
    bool getRunOnStop(void);

    /*写入数据*/
    int sendMainMsg(const char *pszBuff, const size_t &iSize);
    int sendAssistMsg(const char *pszBuff, const size_t &iSize);
    /*获取event_base*/
    struct event_base *getBase(void)
    {
        return m_pBase;
    };
    CEventBuffer *getMainBuffer(void)
    {
        return &m_objMainBuffer;
    };
    CEventBuffer *getAssistBuffer(void)
    {
        return &m_objAssistBuffer;
    };

public:
    virtual void onMainRead(CEventBuffer *){};
    virtual void onAssistRead(CEventBuffer *){};
    virtual void onStop(void)
    {
        setRunStatus(RunStatus_Stopped);
    };
    virtual bool onStartUp(void)
    {
        return true;
    };
public:
    static void mainReadCB(struct bufferevent *bev, void *arg);
    static void assistReadCB(struct bufferevent *bev, void *arg);
    static void eventCB(struct bufferevent *bev, short event, void *arg);
    static void exitMonitorCB(evutil_socket_t, short event, void *arg);

private:
    void freeAll(void);
    int initMainEvent(void);
    int initAssistEvent(void);
    int initExitMonitor(unsigned int uiMS);
    bool getError(void);
    bool getIsRun(void);

private:
    bool m_bRunOnStop;
    RunStatus m_emStatus;
    struct event_base *m_pBase;
    struct bufferevent *m_pMainBev;
    struct bufferevent *m_pAssistBev;
    struct event *m_pExitEvent;
    CSockPair m_objMainSockPair;
    CSockPair m_objAssistSockPair;
    CEventBuffer m_objMainBuffer; 
    CEventBuffer m_objAssistBuffer; 
    CQMutex m_Mutex;
    CCond m_Cond;
};

/*从buffer中读取.返回指针需要手动删除*/
template<class T>
T *Q_GetEventValue(CEventBuffer *pBuffer)
{
    size_t iSize = sizeof(T);

    size_t iTotalSize = pBuffer->getTotalLens();
    if (iSize > iTotalSize)
    {
        return NULL;
    }

    T *pDate = new(std::nothrow) T();
    if (NULL == pDate)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

        return NULL;
    }

    char *pTmp = pBuffer->readBuffer(iSize);
    *pDate = *((T *)pTmp);
    pBuffer->delBuffer(iSize);

    return pDate;
}

/*从buffer中读取*/
template<class T>
bool Q_GetEventValue(CEventBuffer *pBuffer, T &Date)
{
    size_t iSize = sizeof(T);

    size_t iTotalSize = pBuffer->getTotalLens();
    if (iSize > iTotalSize)
    {
        return false;
    }

    char *pTmp = pBuffer->readBuffer(iSize);
    Date = *((T*)pTmp);
    pBuffer->delBuffer(iSize);

    return true;
}

#endif//Q_SOCKPAIREVENT_H_
