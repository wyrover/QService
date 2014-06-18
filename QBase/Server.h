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
    /*监听端口*/
    void setPort(const unsigned short usPort);
    unsigned short getPort(void);
    /*设置接口对象组，数目与线程usThreadNum相同*/
    void setInterface(std::vector<CEventInterface * > &lstInterface);
    /*定时器时间*/
    void setTimer(unsigned int uiMS = 0);
    unsigned int getTimer(void);      
    /*绑定IP*/
    void setBindIP(const char *pszBindIP = "0.0.0.0");
    const char *getBindIP(void);

    /*是否运行*/
    bool getIsRun(void);
    /*等待启动完成*/
    bool waitForStarted(void); 
    /*获取工作线程事件对象*/
    CWorkThreadEvent *getServerThreadEvent(void);
    /*是否发送错误*/
    void setError(bool bError);
    bool getError(void);

    /*启动服务*/
    int Start(void);
    /*停止服务*/
    void Stop(void);

public:
    static void listenerAcceptCB(struct evconnlistener *, Q_SOCK sock, struct sockaddr *, 
        int iSockLen, void *arg);
    static void mainLoopExitCB(struct bufferevent *bev, void *arg);

private:
    int initMainListener(void);
    int initWorkThread(void);
    int Loop(void);
    void exitWorkThread(void);
    void freeMainEvent(void);

private:    
    bool m_bShutDownNormal;
    bool m_bLoop;
    bool m_bError;
    unsigned short m_usThreadNum;
    unsigned short m_usPort;
    unsigned int m_uiTimer;
    struct evconnlistener *m_pListener;
    struct event_base *m_pMainBase;
    struct bufferevent *m_pEvent_Exit;
    class CThreadPool *m_pPool;
    CWorkThreadEvent *m_pServerThreadEvent;
    CSockPair *m_pPair_Exit;
    std::vector<CEventInterface * > m_vcInterface;
    CMutex m_objMutex_Exit;
    CCond m_objCond_Exit;
    std::string m_strBindIP;
};

#endif//Q_SERVER_H_
