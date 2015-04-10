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
    m_cRunStatus = RunStatus_Unknown;
    m_usThreadNum = 1;
    m_usPort = Q_INIT_NUMBER;
    m_usHttpPort = Q_INIT_NUMBER;
    m_usWebSockPort = Q_INIT_NUMBER;
    m_httpSock = Q_INVALID_SOCK;
    m_pListener = NULL;
    m_pWebSockListener = NULL;
    m_pMainBase = NULL;
    m_pPool = NULL;
    m_pServerThreadEvent = NULL;
    m_pExitEvent = NULL;
}

CServer::~CServer(void)
{
    Q_SafeDelete_Array(m_pServerThreadEvent);
    Q_SafeDelete(m_pPool);
    freeMainEvent();
    if (Q_INVALID_SOCK != m_httpSock)
    {
        evutil_closesocket(m_httpSock);
        m_httpSock = Q_INVALID_SOCK;
    }
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

void CServer::setWebSockBindIP(const char *pszBindIP)
{
    m_strWebSockBindIP = pszBindIP;
}

const char *CServer::getWebSockBindIP(void)
{
    return m_strWebSockBindIP.c_str();
}

void CServer::setWebSockPort(const unsigned short usPort)
{
    m_usWebSockPort = usPort;
}

unsigned short CServer::getWebSockPort(void)
{
    return m_usWebSockPort;
}

bool CServer::getError(void)
{
    return ((RunStatus_Error == getRunStatus()) ? true : false);;
}

void CServer::setRunStatus(RunStatus emStatus)
{
    m_cRunStatus = emStatus;
}

RunStatus CServer::getRunStatus(void)
{
    return (RunStatus)m_cRunStatus;
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
    return ((RunStatus_Runing == getRunStatus()) ? true : false);
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

void CServer::webSockAcceptCB(struct evconnlistener *, Q_SOCK sock, struct sockaddr *, 
    int iSockLen, void *arg)
{
    unsigned short usIndex = Q_INIT_NUMBER;  
    CServer *pServer = (CServer *)arg;
    CWorkThreadEvent *pThreadEvent = pServer->getServerThreadEvent();

    if (RunStatus_Runing != pServer->getRunStatus())
    {
        evutil_closesocket(sock);
        return;
    }

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

    if (Q_RTN_OK != pThreadEvent[usIndex].sendWebSockMsg((const char*)&sock, sizeof(sock)))
    {
        Q_Printf("add socket %d to thread %d error.", sock, usIndex);
        Q_SYSLOG(LOGLV_ERROR, "add socket %d to thread %d error.", sock, usIndex);

        evutil_closesocket(sock);
    }
}

void CServer::listenerAcceptCB(struct evconnlistener *, Q_SOCK sock, struct sockaddr *, 
    int iSockLen, void *arg)
{
    unsigned short usIndex = Q_INIT_NUMBER;  
    CServer *pServer = (CServer *)arg;
    CWorkThreadEvent *pThreadEvent = pServer->getServerThreadEvent();

    if (RunStatus_Runing != pServer->getRunStatus())
    {
        evutil_closesocket(sock);
        return;
    }

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

    if (Q_RTN_OK != pThreadEvent[usIndex].sendTcpMsg((const char*)&sock, sizeof(sock)))
    {
        Q_Printf("add socket %d to thread %d error.", sock, usIndex);
        Q_SYSLOG(LOGLV_ERROR, "add socket %d to thread %d error.", sock, usIndex);

        evutil_closesocket(sock);
    }
}

void CServer::exitMonitorCB(evutil_socket_t, short event, void *arg)
{
    CServer *pServer = (CServer *)arg;
    switch(pServer->getRunStatus())
    {
    case RunStatus_Stopping:
        {
            Q_Printf("ready stop main thread.");
            pServer->setRunStatus(RunStatus_Stopped);            
        }
        break;

    case RunStatus_Stopped:
        {
            Q_Printf("stop main thread successfully.");
            event_base_loopbreak(pServer->getBase());
        }
        break;

    default:
        break;
    }
}

int CServer::initWebSockListener(void)
{
    int iRtn = Q_RTN_OK;
    CNETAddr objAddr;

    iRtn = objAddr.setAddr(getWebSockBindIP(), getWebSockPort());
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "set network addr error.");

        return iRtn;
    }

    m_pWebSockListener = evconnlistener_new_bind(m_pMainBase, webSockAcceptCB, this, 
        LEV_OPT_CLOSE_ON_FREE, -1, 
        objAddr.getAddr(), objAddr.getAddrSize());
    if (NULL == m_pWebSockListener)
    {
        iRtn = EVUTIL_SOCKET_ERROR();
        Q_Printf("evconnlistener_new_bind error. error code %d, message %s", 
            iRtn, evutil_socket_error_to_string(iRtn));

        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
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

    m_pListener = evconnlistener_new_bind(m_pMainBase, listenerAcceptCB, this, 
        LEV_OPT_CLOSE_ON_FREE, -1, 
        objAddr.getAddr(), objAddr.getAddrSize());
    if (NULL == m_pListener)
    {
        iRtn = EVUTIL_SOCKET_ERROR();
        Q_Printf("evconnlistener_new_bind error. error code %d, message %s", 
            iRtn, evutil_socket_error_to_string(iRtn));

        return Q_RTN_FAILE;
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
    if (NULL == m_pServerThreadEvent)
    {
        return;
    }

    for (int i = 0; i < getThreadNum(); i++)
    {
        m_pServerThreadEvent[i].Stop();
    }
}

void CServer::freeMainEvent(void)
{
    if (NULL != m_pListener)
    {
        evconnlistener_free(m_pListener);
        m_pListener = NULL;
    }

    if (NULL != m_pWebSockListener)
    {
        evconnlistener_free(m_pWebSockListener);
        m_pWebSockListener = NULL;
    }

    if (NULL != m_pExitEvent)
    {
        event_free(m_pExitEvent);
        m_pExitEvent = NULL;
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
            setRunStatus(RunStatus_Error);
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

            return Q_ERROR_ALLOCMEMORY;
        }

        pTask->setWorkerHandle(&m_pServerThreadEvent[us]);
        m_pPool->Append(pTask, Q_ThreadLV_High);
        if (!m_pServerThreadEvent[us].waitForStarted())
        {
            setRunStatus(RunStatus_Error);
            Q_Printf("%s", "wait work thread start error.");

            return Q_RTN_FAILE;
        }
    }

    setRunStatus(RunStatus_Runing);
    iRtn = event_base_dispatch(m_pMainBase);
    setRunStatus(RunStatus_Stopped);

    m_objMutex_Exit.Lock();
    m_objCond_Exit.Signal();
    m_objMutex_Exit.unLock();

    return iRtn;
}

int CServer::initExitMonitor(unsigned int uiMS)
{
    timeval tVal;
    evutil_timerclear(&tVal);
    if (uiMS >= 1000)
    {
        tVal.tv_sec = uiMS / 1000;
        tVal.tv_usec = (uiMS % 1000) * (1000);
    }
    else
    {
        tVal.tv_usec = (uiMS * 1000);
    }

    m_pExitEvent = event_new(m_pMainBase, 
        -1, EV_PERSIST, exitMonitorCB, this);
    if (NULL == m_pExitEvent)
    {
        Q_Printf("%s", "event_new error");

        return Q_RTN_FAILE;
    }

    if (Q_RTN_OK != event_add(m_pExitEvent, &tVal))
    {
        Q_Printf("%s", "event_add error");

        event_free(m_pExitEvent);
        m_pExitEvent = NULL;

        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

int CServer::Init(void)
{
    int iRtn = Q_RTN_OK;

    setRunStatus(RunStatus_Starting);
    /*主线程循环*/
    m_pMainBase = event_base_new();
    if (NULL == m_pMainBase)
    {
        Q_Printf("%s", "event_base_new error.");
        setRunStatus(RunStatus_Error);

        return Q_RTN_FAILE;
    }

    /*初始化工作线程*/
    Q_Printf("%s", "init work thread...");
    iRtn = initWorkThread();
    if (Q_RTN_OK != iRtn)
    {
        setRunStatus(RunStatus_Error);

        return iRtn;
    }

    /*初始化HTTP*/
    if(0 != m_usHttpPort)
    {
        Q_Printf("%s", "init http server...");
        m_httpSock = initHttpSock();
        if (Q_INVALID_SOCK == m_httpSock)
        {
            setRunStatus(RunStatus_Error);

            return Q_RTN_FAILE;
        }
    }

    /*初始化websock监听*/
    if (0 != m_usWebSockPort)
    {
        Q_Printf("%s", "init websock server...");
        iRtn = initWebSockListener();
        if (Q_RTN_OK != iRtn)
        {
            setRunStatus(RunStatus_Error);

            return Q_RTN_FAILE;
        }
    }    

    /*初始化tcp监听*/
    if (0 != m_usPort)
    {
        Q_Printf("%s", "init tcp listener...");
        iRtn = initMainListener();
        if (Q_RTN_OK != iRtn)
        {
            setRunStatus(RunStatus_Error);

            return iRtn;
        }
    }

    /*初始化退出*/
    iRtn = initExitMonitor(100);
    if (Q_RTN_OK != iRtn)
    {
        setRunStatus(RunStatus_Error);

        return iRtn;
    }

    try
    {
        m_pPool = new(std::nothrow) CThreadPool(getThreadNum());
        if (NULL == m_pPool)
        {
            setRunStatus(RunStatus_Error);
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

            return Q_ERROR_ALLOCMEMORY;
        }
    }
    catch(CException &e)
    {
        setRunStatus(RunStatus_Error);
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
    if (!getIsRun())
    {
        exitWorkThread();

        return;
    }
    
    exitWorkThread();

    m_objMutex_Exit.Lock();
    setRunStatus(RunStatus_Stopping);
    m_objCond_Exit.Wait(&m_objMutex_Exit);    
    m_objMutex_Exit.unLock();
}
