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

#include "SockPairEvent.h"
#include "SessionManager.h"
#include "WebSockParser.h"
#include "TcpParser.h"

struct TriggerSock
{
    SessionType emType;
    Q_SOCK iSock;
    TriggerSock(void) : iSock(Q_INVALID_SOCK)
    {
        
    };
};

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
    int setHttpSock(std::vector<Q_SOCK> &vcHttpSock);

    /*获取该事件循环维护的CSessionManager*/
    CSessionManager *getSessionManager(void);
    /*获取WebSockParser*/
    CWebSockParser *getWebSockParser(void);
    /*获取CTcpParser*/
    CTcpParser *getTcpParser(void);
    /*获取链接到其他服务器对象*/
    CLinkOther *getLinkOther(void);

public:
    //接口实现
    void onMainRead(CEventBuffer *pBuffer);
    void onAssistRead(CEventBuffer *pBuffer);
    void onStop(void);
    bool onStartUp(void);

public:
    static void workThreadReadCB(struct bufferevent *bev, void *arg);
    static void workThreadEventCB(struct bufferevent *bev, short event, void *arg);
    static void workThreadTimerCB(evutil_socket_t, short event, void *arg);
    static void workThreadHttpCB(struct evhttp_request *req, void *arg);

    //不同类型协议读取
    static void dispTcp(CWorkThreadEvent *pWorkThreadEvent,
        CSessionManager *pSessionManager, CSession *pSession);
    static void dispWebSock(CWorkThreadEvent *pWorkThreadEvent,
        CSessionManager *pSessionManager, CSession *pSession);

public:
    //websocket 分片帧操作
    void addContinuation(const Q_SOCK &sock, const char *pszData, const size_t &iLens);
    std::string *getContinuation(const Q_SOCK &sock);
    void delContinuation(const Q_SOCK &sock);

private:
    struct event *m_pEvent;
    CSessionManager m_objSessionManager;
    TriggerSock m_stWorkSock;
    CWebSockParser m_objWebSockParser;
    CTcpParser m_objTcpParser;
    std::vector<struct evhttp *> m_vcEvHttp;
    std::tr1::unordered_map<Q_SOCK, std::string> m_mapWebSockPack;
};

#endif//Q_SERVERTHREAD_EVENT_H_
