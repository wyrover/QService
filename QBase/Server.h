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

#ifndef Q_SERVER_H_
#define Q_SERVER_H_

#include "WorkThreadEvent.h"

class CServer
{
public:
    CServer(void);
    ~CServer(void);

    /*等待启动完成*/
    bool waitForStarted(void); 
    /*获取工作线程事件对象*/
    CWorkThreadEvent *getServerThreadEvent(void);
    /*状态设置*/
    void setRunStatus(RunStatus emStatus);
    RunStatus *getRunStatus(void);
    /*获取类型*/
    SessionType getSockType(evutil_socket_t uiSock);
    /*获取线程数*/
    unsigned short *getThreadNum(void)
    {
        return &m_usThreadNum;
    };
    
    struct event_base *getBase(void)
    {
        return m_pMainBase;
    };

    /*初始化*/
    int Init(const unsigned short &usThreadNum, const unsigned int &uiTime,
        std::vector<CEventInterface * > &vcInterface,
        std::map<unsigned short, std::string> &mapTcp, 
        std::map<unsigned short, std::string> &mapWebSock, 
        std::map<unsigned short, std::string> &mapHttp);
    /*启动服务*/
    int Start(void);
    /*停止服务*/
    void Stop(void);

    TriggerSock *getTriggerSock(void)
    {
        return &m_stWorkSock;
    };

public:
    static void listenerAcceptCB(struct evconnlistener *pListener, Q_SOCK sock, struct sockaddr *, 
        int iSockLen, void *arg);
    static void exitMonitorCB(evutil_socket_t, short event, void *arg);

private:
    struct evconnlistener * initListener(const char *pszHost, const unsigned short usPort);
    int initWorkThread(std::vector<CEventInterface * > &vcInterface);
    int Loop(void);
    void exitWorkThread(void);
    void freeMainEvent(void);
    bool getError(void);
    bool getIsRun(void);
    int initExitMonitor(unsigned int uiMS);
    Q_SOCK initHttpSock(const char *pszIp, const unsigned short &usPort);

private:
    RunStatus m_emRunStatus;
    unsigned short m_usThreadNum;
    unsigned int m_uiTimer;
    struct event_base *m_pMainBase;
    class CThreadPool *m_pPool;
    CWorkThreadEvent *m_pServerThreadEvent;
    struct event *m_pExitEvent;
    CQMutex m_objMutex_Exit;
    CCond m_objCond_Exit;
    TriggerSock m_stWorkSock;
    std::vector<Q_SOCK> m_vcHttpSock;
    std::vector<struct evconnlistener * > m_vcAllListener;
    std::map<evutil_socket_t, SessionType> m_mapType;
};

#endif//Q_SERVER_H_
