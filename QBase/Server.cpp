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

#include "Server.h"
#include "ThreadPool.h"
#include "Exception.h"
#include "SysLog.h"

#define EXIT_COMMAND "EXIT"

class CWorkerThreadTask : public CTask
{
public:
    CWorkerThreadTask(void) : m_pThreadEvent(NULL)
    {};
    ~CWorkerThreadTask(void){};

    void setWorkerHandle(CWorkThreadEvent *pThreadEvent)
    {
        m_pThreadEvent = pThreadEvent;
    };

    void Run(void)
    {
        m_pThreadEvent->Start();
    }

private:
    CWorkThreadEvent *m_pThreadEvent;
};

/*****************************************************************************/

CServer::CServer(void)
{
    m_uiTimer = Q_INIT_NUMBER;
    m_bShutDownNormal = false;
    m_bLoop = false;
    m_bError = false;
    m_usThreadNum = 1;
    m_usPort = 0;
    m_usHttpPort = 0;
    m_httpSock = Q_INVALID_SOCK;
    m_pListener = NULL;
    m_pMainBase = NULL;
    m_pEvent_Exit = NULL;
    m_pPool = NULL;
    m_pServerThreadEvent = NULL;
    m_pPair_Exit = NULL;
}

CServer::~CServer(void)
{
    Stop();
}

void CServer::setThreadNum(const unsigned short usThreadNum)
{
    m_usThreadNum = ((usThreadNum == 0) ? 1 : usThreadNum);
}

unsigned short CServer::getThreadNum(void)
{
    return m_usThreadNum;
}

void CServer::setPort(const unsigned short usPort)
{
    m_usPort = usPort;
}

unsigned short CServer::getPort(void)
{
    return m_usPort;
}

void CServer::setHttpBindIP(const char *pszBindIP)
{
    m_strHttpBindIP = pszBindIP;
}

const char *CServer::getHttpBindIP(void)
{
    return m_strHttpBindIP.c_str();
}

void CServer::setHttpPort(const unsigned short usPort)
{
    m_usHttpPort = usPort;
}

unsigned short CServer::getHttpPort(void)
{
    return m_usHttpPort;
}

void CServer::setError(bool bError)
{
    m_bError = bError;
}

bool CServer::getError(void)
{
    return m_bError;
}

void CServer::setInterface(std::vector<CEventInterface * > &lstInterface)
{
    m_vcInterface = lstInterface;
}

void CServer::setTimer(unsigned int uiMS)
{
    m_uiTimer = uiMS;
}

unsigned int CServer::getTimer(void)
{
    return m_uiTimer;
}

bool CServer::getIsRun(void)
{
    return m_bLoop;
}

bool CServer::waitForStarted(void)
{
    while(true)
    {
        if (getIsRun())
        {
            return true;
        }

        if (getError())
        {
            return false;
        }

        Q_Sleep(10);
    }
}

void CServer::setBindIP(const char *pszBindIP)
{
    m_strBindIP = pszBindIP;
}

const char *CServer::getBindIP(void)
{
    return m_strBindIP.c_str();
}

CWorkThreadEvent *CServer::getServerThreadEvent(void)
{
    return m_pServerThreadEvent;
}

void CServer::listenerAcceptCB(struct evconnlistener *, Q_SOCK sock, struct sockaddr *, 
    int iSockLen, void *arg)
{
    unsigned short usIndex = Q_INIT_NUMBER;  
    CServer *pServer = (CServer *)arg;
    CWorkThreadEvent *pThreadEvent = pServer->getServerThreadEvent();

    //取得最小连接数的线程号
    if (pServer->getThreadNum() > 1)
    {
        unsigned int uiCount = Q_INIT_NUMBER;
        unsigned int uiTmp = pThreadEvent[0].getSessionManager()->getSessionSize();
        for (unsigned short usI = Q_INIT_NUMBER; usI < pServer->getThreadNum(); usI++)
        {
            uiCount = pThreadEvent[usI].getSessionManager()->getSessionSize();
            if (uiTmp > uiCount)
            {
                uiTmp = uiCount;
                usIndex = usI;
            }
        }
    }    

    if (Q_RTN_OK != pThreadEvent[usIndex].sendMainMsg((const char*)&sock, sizeof(sock)))
    {
        Q_Printf("add socket %d to thread %d error.", sock, usIndex);
        Q_SYSLOG(LOGLV_ERROR, "add socket %d to thread %d error.", sock, usIndex);

        evutil_closesocket(sock);
    }
}

void CServer::mainLoopExitCB(struct bufferevent *bev, void *arg)
{
    size_t iSize = Q_INIT_NUMBER;
    std::string strTmp;
    CEventBuffer objBuffer;
    struct event_base *pMainBase = NULL;

    pMainBase = (struct event_base *)arg;

    if (Q_RTN_OK != objBuffer.setBuffer(bev))
    {
        return;
    }

    iSize = objBuffer.getTotalLens();
    if (strlen(EXIT_COMMAND) <= iSize)
    {
        event_base_loopbreak(pMainBase);

        Q_Printf("%s", "get order to exit main loop");
    }
}

int CServer::initMainListener(void)
{
    int iRtn = Q_RTN_OK;
    CNETAddr objAddr;

    iRtn = objAddr.setAddr(getBindIP(), getPort());
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "set network addr error.");

        return iRtn;
    }

    m_pMainBase = event_base_new();
    if (NULL == m_pMainBase)
    {
        Q_Printf("%s", "event_base_new error.");

        return Q_RTN_FAILE;
    }

    m_pListener = evconnlistener_new_bind(m_pMainBase, listenerAcceptCB, this, 
        LEV_OPT_CLOSE_ON_FREE, -1, 
        objAddr.getAddr(), objAddr.getAddrSize());
    if (NULL == m_pListener)
    {
        iRtn = EVUTIL_SOCKET_ERROR();
        Q_Printf("evconnlistener_new_bind error. error code %d, message %s", 
            iRtn, evutil_socket_error_to_string(iRtn));

        return iRtn;
    }

    try
    {
        m_pPair_Exit = new(std::nothrow) CSockPair();
        if (NULL == m_pPair_Exit)
        {
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

            return Q_ERROR_ALLOCMEMORY;
        }
    }
    catch(CException &e)
    {
        Q_Printf("get an exception. code %d, message %s", e.getErrorCode(), e.getErrorMsg());
        
        return e.getErrorCode();
    }

    m_pEvent_Exit = bufferevent_socket_new(m_pMainBase, 
        m_pPair_Exit->getReadFD(), BEV_OPT_CLOSE_ON_FREE);
    if (NULL == m_pEvent_Exit)
    {
        Q_Printf("%s", "bufferevent_socket_new error.");

        return Q_RTN_FAILE;
    }

    bufferevent_setcb(m_pEvent_Exit, mainLoopExitCB, NULL, NULL, m_pMainBase);
    iRtn = bufferevent_enable(m_pEvent_Exit, EV_READ);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "bufferevent_enable error.");

        return iRtn;
    }

    return Q_RTN_OK;
}

Q_SOCK CServer::initHttpSock(void)
{
    CNETAddr objAddr;
    Q_SOCK sock = Q_INVALID_SOCK;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (Q_INVALID_SOCK == sock)
    {
        Q_Printf("%s", "create socket error.");
        return Q_INVALID_SOCK;
    }

    if (Q_RTN_OK != evutil_make_listen_socket_reuseable(sock))
    {
        Q_Printf("%s", "make socket reuseable error.");
        evutil_closesocket(sock);
        return Q_INVALID_SOCK;
    }

    objAddr.setAddr(m_strHttpBindIP.c_str(), m_usHttpPort);
    if (bind(sock, objAddr.getAddr(), objAddr.getAddrSize()) < 0)
    {
        Q_Printf("bind socket on port: %d ip: %s error.", m_usHttpPort, m_strHttpBindIP.c_str());
        evutil_closesocket(sock);
        return Q_INVALID_SOCK;
    }

    if (listen(sock, 5) < 0)
    {
        Q_Printf("%s", "listen socket error.");
        evutil_closesocket(sock);
        return Q_INVALID_SOCK;
    }

    if (Q_RTN_OK != evutil_make_socket_nonblocking(sock))
    {
        Q_Printf("%s", "make socket nonblock error.");
        evutil_closesocket(sock);
        return Q_INVALID_SOCK;
    }
    
    return sock;
}

int CServer::initWorkThread(void)
{
    try
    {
        m_pServerThreadEvent = new(std::nothrow) CWorkThreadEvent[getThreadNum()];
        if (NULL == m_pServerThreadEvent)
        {
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

            return Q_ERROR_ALLOCMEMORY;
        }
    }
    catch(CException &e)
    {
        Q_Printf("get an exception. code %d, message %s", e.getErrorCode(), e.getErrorMsg());

        return e.getErrorCode();
    }

    for (unsigned short usI = 0; usI < getThreadNum(); usI++)
    {
        m_pServerThreadEvent[usI].getSessionManager()->setThreadIndex(usI);
        m_pServerThreadEvent[usI].setInterface(m_vcInterface[usI]);         
        if (Q_RTN_OK != m_pServerThreadEvent[usI].setTimer(getTimer()))
        {
            return Q_RTN_FAILE;
        }
        if (Q_RTN_OK != m_pServerThreadEvent[usI].setHttp(m_httpSock))
        {
            return Q_RTN_FAILE;
        }
    }

    return Q_RTN_OK;
}

void CServer::exitWorkThread(void)
{
    Q_SafeDelete_Array(m_pServerThreadEvent);
}

void CServer::freeMainEvent(void)
{
    if (NULL != m_pListener)
    {
        evconnlistener_free(m_pListener);
        m_pListener = NULL;
    }

    Q_SafeDelete(m_pPair_Exit);

    if (NULL != m_pEvent_Exit)
    {
        bufferevent_free(m_pEvent_Exit);
        m_pEvent_Exit = NULL;
    }

    if (NULL != m_pMainBase)
    {
        event_base_free(m_pMainBase);
        m_pMainBase = NULL;
    }
}

int CServer::Loop(void)
{
    int iRtn = Q_RTN_OK;
    CWorkerThreadTask *pTask = NULL;

    for (unsigned short us = 0; us < getThreadNum(); us++)
    {
        pTask = new(std::nothrow) CWorkerThreadTask();
        if (NULL == pTask)
        {
            setError(true);
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

            return Q_ERROR_ALLOCMEMORY;
        }

        pTask->setWorkerHandle(&m_pServerThreadEvent[us]);
        m_pPool->Append(pTask, Q_ThreadLV_High);
        if (!m_pServerThreadEvent[us].waitForStarted())
        {
            setError(true);
            Q_Printf("%s", "wait work thread start error.");

            return Q_RTN_FAILE;
        }
    }

    m_bLoop = true;
    Q_Printf("%s", "run main loop");
    iRtn = event_base_dispatch(m_pMainBase);
    if (!m_bShutDownNormal)
    {
        iRtn = EVUTIL_SOCKET_ERROR();

        Q_Printf("happen error on loop. error code %d, message %s", 
            iRtn, evutil_socket_error_to_string(iRtn));
        Q_SYSLOG(LOGLV_ERROR, "happen error on loop. error code %d, message %s", 
            iRtn, evutil_socket_error_to_string(iRtn));

        setError(true);

        g_objExitMutex.Lock();
        g_objExitCond.Signal();
        g_objExitMutex.unLock();
    }
    Q_Printf("%s", "exit main loop");
    m_bLoop = false;

    m_objMutex_Exit.Lock();
    m_objCond_Exit.Signal();
    m_objMutex_Exit.unLock();

    return iRtn;
}

int CServer::Init(void)
{
    int iRtn = Q_RTN_OK;

    m_bShutDownNormal = false;
    Q_Printf("%s", "start server...");

    /*初始化HTTP*/
    if(0 != m_usHttpPort)
    {
        Q_Printf("%s", "init http server...");
        m_httpSock = initHttpSock();
        if (Q_INVALID_SOCK == m_httpSock)
        {
            return Q_RTN_FAILE;
        }
    }

    /*初始化工作线程*/
    iRtn = initWorkThread();
    if (Q_RTN_OK != iRtn)
    {
        setError(true);

        return iRtn;
    }

    /*初始化监听线程*/
    iRtn = initMainListener();
    if (Q_RTN_OK != iRtn)
    {
        setError(true);

        return iRtn;
    }

    try
    {
        m_pPool = new(std::nothrow) CThreadPool(getThreadNum());
        if (NULL == m_pPool)
        {
            setError(true);
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

            return Q_ERROR_ALLOCMEMORY;
        }
    }
    catch(CException &e)
    {
        setError(true);
        Q_Printf("get an exception. code %d, message %s", e.getErrorCode(), e.getErrorMsg());

        return e.getErrorCode();
    }

    return Q_RTN_OK;
}

int CServer::Start(void)
{
    /*启动循环*/
    return Loop();
}

void CServer::Stop(void)
{
    Q_Printf("%s", "shutdown server...");
    if (!getIsRun())
    {
        exitWorkThread();
        freeMainEvent();
        Q_SafeDelete(m_pPool);

        return;
    }

    m_bShutDownNormal = true;
    
    exitWorkThread();
    Q_SafeDelete(m_pPool);

    Q_Printf("%s", "wait main event loop stop");

    m_objMutex_Exit.Lock();    
    (void)m_pPair_Exit->Write(EXIT_COMMAND, strlen(EXIT_COMMAND));
    m_objCond_Exit.Wait(&m_objMutex_Exit);    
    m_objMutex_Exit.unLock();

    Q_Printf("%s", "event main loop stoped");

    freeMainEvent();
}
