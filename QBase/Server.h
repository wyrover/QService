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

    /*工作线程数*/
    void setThreadNum(const unsigned short usThreadNum = 1);
    unsigned short getThreadNum(void);    
    /*设置接口对象组，数目与线程usThreadNum相同*/
    void setInterface(std::vector<CEventInterface * > &lstInterface);
    /*定时器时间*/
    void setTimer(unsigned int uiMS = 0);
    unsigned int getTimer(void);      
    /*tcp绑定IP*/
    void setBindIP(const char *pszBindIP = "0.0.0.0");
    const char *getBindIP(void);
    /*tcp监听端口*/
    void setPort(const unsigned short usPort);
    unsigned short getPort(void);
    /*http绑定IP*/
    void setHttpBindIP(const char *pszBindIP = "0.0.0.0");
    const char *getHttpBindIP(void);
    /*http监听端口*/
    void setHttpPort(const unsigned short usPort);
    unsigned short getHttpPort(void);
    /*websock绑定IP*/
    void setWebSockBindIP(const char *pszBindIP = "0.0.0.0");
    const char *getWebSockBindIP(void);
    /*websock监听端口*/
    void setWebSockPort(const unsigned short usPort);
    unsigned short getWebSockPort(void);

    /*等待启动完成*/
    bool waitForStarted(void); 
    /*获取工作线程事件对象*/
    CWorkThreadEvent *getServerThreadEvent(void);
    /*状态设置*/
    void setRunStatus(RunStatus emStatus);
    RunStatus getRunStatus(void);
    
    struct event_base *getBase(void)
    {
        return m_pMainBase;
    };

    /*初始化*/
    int Init(void);
    /*启动服务*/
    int Start(void);
    /*停止服务*/
    void Stop(void);

public:
    static void listenerAcceptCB(struct evconnlistener *, Q_SOCK sock, struct sockaddr *, 
        int iSockLen, void *arg);
    static void webSockAcceptCB(struct evconnlistener *, Q_SOCK sock, struct sockaddr *, 
        int iSockLen, void *arg);
    static void exitMonitorCB(evutil_socket_t, short event, void *arg);

private:
    int initMainListener(void);
    int initWebSockListener(void);
    int initWorkThread(void);
    int Loop(void);
    void exitWorkThread(void);
    void freeMainEvent(void);
    Q_SOCK initHttpSock(void);
    bool getError(void);
    bool getIsRun(void);
    int initExitMonitor(unsigned int uiMS);

private:
    char m_cRunStatus;
    unsigned short m_usThreadNum;
    unsigned short m_usPort;
    unsigned short m_usHttpPort;
    unsigned short m_usWebSockPort;
    unsigned int m_uiTimer;
    Q_SOCK m_httpSock;
    struct evconnlistener *m_pListener;
    struct evconnlistener *m_pWebSockListener;
    struct event_base *m_pMainBase;
    class CThreadPool *m_pPool;
    CWorkThreadEvent *m_pServerThreadEvent;
    struct event *m_pExitEvent;
    std::vector<CEventInterface * > m_vcInterface;
    CMutex m_objMutex_Exit;
    CCond m_objCond_Exit;
    std::string m_strBindIP;
    std::string m_strHttpBindIP;
    std::string m_strWebSockBindIP;
};

#endif//Q_SERVER_H_
