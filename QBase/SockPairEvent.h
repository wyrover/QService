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

enum OrderType
{
    OrderType_AddServerLinker = 0,
};

struct OrderMsg
{
    OrderType emType;
    void *pHandle;
    OrderMsg(void)
    {
        emType = OrderType_AddServerLinker;
        pHandle = NULL;
    }
};

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
    /*获取是否进入事件循环*/
    bool getIsRun(void);
    /*等待进入事件循环*/
    bool waitForStarted(void);
    /*是否发生错误*/
    void setError(bool bError);
    bool getError(void);

    /*主读写sock 参数*/
    void setTcpParam(void *pArg = NULL);
    /*退出循环sock 参数*/
    void setExitParam(void *pArg = NULL);
    /*命令sock参数*/
    void setOrderParam(void *pArg = NULL);
    /*命令sock参数*/
    void setWebSockParam(void *pArg = NULL);
    /*向主读写sock写入数据*/
    int sendTcpMsg(const char *pszBuff, const size_t &iSize);
    /*向命令sock写入数据*/
    int sendOrderMsg(const char *pszBuff, const size_t &iSize);
    /*向命令websock写入数据*/
    int sendWebSockMsg(const char *pszBuff, const size_t &iSize);
    /*获取event_base*/
    struct event_base *getBase(void)
    {
        return m_pEventBase;
    };

public:
    virtual void onTcpRead(struct SockPairEventParam *pParam){};
    virtual void onStop(struct SockPairEventParam *pParam){};
    virtual void onOrderRead(struct SockPairEventParam *pParam){};
    virtual void onWebSockRead(struct SockPairEventParam *pParam){};
    virtual void onStartUp(void){};
public:
    static void sockPairEventReadCB(struct bufferevent *bev, void *arg);
    static void sockPairEventCB(struct bufferevent *bev, short event, void *arg);

private:
    int Init(void);
    void freeAll(void);
    int initEvent(void);
    int initEvent(struct bufferevent **pBev, struct SockPairEventParam *pParam, CSockPair &objPair);

private:
    bool m_bExitNormal;
    bool m_bRun;
    bool m_bError;
    struct event_base *m_pEventBase;
    struct bufferevent **m_pBevs;
    CSockPair *m_pSockPairs;
    CEventBuffer *m_pBuffers;
    struct SockPairEventParam *m_pParam;
    CMutex m_Mutex;
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
