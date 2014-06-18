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

#ifndef Q_SERVERTHREAD_EVENT_H_
#define Q_SERVERTHREAD_EVENT_H_

#include "SessionManager.h"
#include "ServerLinker.h"

/*
* 工作线程事件
* 通信格式 unsigned short(消息长度) + 消息
*/
class CWorkThreadEvent : public CSockPairEvent
{
public:
    CWorkThreadEvent(void);
    ~CWorkThreadEvent(void);

    /*设置接口对象*/
    void setInterface(CEventInterface *pInterface);
    /*定时器时间(ms)*/
    int setTimer(unsigned int uiMS);
    /*获取该事件循环维护的CSessionManager*/
    CSessionManager *getSessionManager(void);

public:
    void onMainRead(struct SockPairEventParam *pParam);
    void onOrderRead(struct SockPairEventParam *pParam);
    void onStop(struct SockPairEventParam *pParam);
    void onStartUp(void);

public:
    static void workThreadReadCB(struct bufferevent *bev, void *arg);
    static void workThreadEventCB(struct bufferevent *bev, short event, void *arg);
    static void workThreadTimerCB(evutil_socket_t, short event, void *arg);

private:
    static char *getDataPack(CSession *pSession, Q_PackHeadType &usSize);
    void addServerLinker(struct event_base *pMainBase, 
        CSessionManager *pSessionManager, OrderMsg &stOrderMsg);

private:
    struct event *m_pEvent;
    CSessionManager m_objSessionManager;
};

#endif//Q_SERVERTHREAD_EVENT_H_