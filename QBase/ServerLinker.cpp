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

#include "ServerLinker.h"
#include "NETAddr.h"
#include "QString.h"
#include "Thread.h"
#include "Exception.h"

class CServerLinkerMonitor : public CTask
{
public:
    CServerLinkerMonitor(void) : m_pLinker(NULL)
    {};
    ~CServerLinkerMonitor(void)
    {};

    void setServerLinker(CServerLinker *pLinker)
    {
        m_pLinker = pLinker;
    };

    void Run(void)
    {
        int iRtn = Q_RTN_OK;

        m_pLinker->setMonitorRun(true);

        m_pLinker->getMutex()->Lock();
        m_pLinker->getCond()->Signal();
        m_pLinker->getMutex()->unLock();

        while(!(m_pLinker->getStop()))
        {
            if (!m_pLinker->getLinked())
            {
                Q_Printf("try link %s on port %d...", m_pLinker->getIp(), m_pLinker->getPort());
                iRtn = TryLink();
                if (Q_RTN_OK != iRtn)
                {
                    Q_Printf("try link %s on port %d error.", 
                        m_pLinker->getIp(), m_pLinker->getPort());
                }
            }

            m_pLinker->getCloseMutex()->Lock();
            m_pLinker->getCloseCond()->Wait(m_pLinker->getCloseMutex(), 5 * 1000);
            m_pLinker->getCloseMutex()->unLock();
        }

        m_pLinker->setMonitorRun(false);

        m_pLinker->getMutex()->Lock();
        m_pLinker->getCond()->Signal();
        m_pLinker->getMutex()->unLock();
    };

private:
    int TryLink()
    {
        int iRtn = Q_RTN_OK;

        iRtn = m_pLinker->Link();
        if (Q_RTN_OK != iRtn)
        {
            m_pLinker->closeSock();

            return iRtn;
        }

        iRtn = m_pLinker->addInLoop();
        if (Q_RTN_OK != iRtn)
        {
            m_pLinker->closeSock();

            return iRtn;
        }

        return Q_RTN_OK;
    };

private:
    CServerLinker *m_pLinker;
};

/********************************************/
CServerLinker::CServerLinker(void) : m_bMonitorRun(false), m_bStop(false), m_bLinked(false), 
    m_usDesPort(Q_INIT_NUMBER), m_iType(0), m_Sock(Q_INVALID_SOCK), m_pEvent(NULL)
{

}

CServerLinker::~CServerLinker(void)
{
    if (getMonitorRun())
    {
        //等待监控退出
        m_objMutex.Lock();
        m_bStop = true;
        m_objMutex_Close.Lock();
        m_objCond_Close.Signal();
        m_objMutex_Close.unLock();
        m_objCond.Wait(&m_objMutex, 1000);
        m_objMutex.unLock();
    }
    
    m_bStop = true;
}

void CServerLinker::setLinked(bool bLinked)
{
    m_bLinked = bLinked;
}

bool CServerLinker::getLinked(void)
{
    return m_bLinked;
}

void CServerLinker::setPort(unsigned short usPort)
{
    m_usDesPort = usPort;
}

unsigned short CServerLinker::getPort(void)
{
    return m_usDesPort;
}

void CServerLinker::setIp(const char *pszIp)
{
    m_strDesIp = pszIp;
}

const char *CServerLinker::getIp(void)
{
    return m_strDesIp.c_str();
}

void CServerLinker::setType(const int iType)
{
    m_iType = iType;
}

int CServerLinker::getType(void)
{
    return m_iType;
}

void CServerLinker::setLinkerName(const char *pszName)
{
    m_strLinkerName = pszName;
}

const char *CServerLinker::getLinkerName(void)
{
    return m_strLinkerName.c_str();
}

void CServerLinker::setSockPairEvent(CSockPairEvent *pEvent)
{
    m_pEvent = pEvent;
}

void CServerLinker::setTimeOut(const unsigned int uiMS)
{
    if (uiMS <= 0)
    {
        return;
    }

#ifdef Q_OS_WIN32
    (void)setsockopt(m_Sock, SOL_SOCKET, SO_SNDTIMEO, (char*)(&uiMS), sizeof(uiMS));  
    (void)setsockopt(m_Sock, SOL_SOCKET, SO_RCVTIMEO, (char*)(&uiMS), sizeof(uiMS));   
#else
    struct timeval  tVal;
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

    (void)setsockopt(m_Sock, SOL_SOCKET, SO_SNDTIMEO, (char*)(&tVal), sizeof(tVal)); 
    (void)setsockopt(m_Sock, SOL_SOCKET, SO_RCVTIMEO, (char*)(&tVal), sizeof(tVal));
#endif
    
    return;
}

void CServerLinker::setNonblock(void)
{
    (void)evutil_make_socket_nonblocking(m_Sock);
}

Q_SOCK CServerLinker::getSock(void)
{
    return m_Sock;
}

CMutex *CServerLinker::getCloseMutex(void)
{
    return &m_objMutex_Close;
}

CCond *CServerLinker::getCloseCond(void)
{
    return &m_objCond_Close;
}

CMutex *CServerLinker::getMutex(void)
{
    return &m_objMutex;
}

CCond *CServerLinker::getCond(void)
{
    return &m_objCond;
}

bool CServerLinker::getStop(void)
{
    return m_bStop;
}

bool CServerLinker::getMonitorRun(void)
{
    return m_bMonitorRun;
}

void CServerLinker::setMonitorRun(bool bMonitorRun)
{
    m_bMonitorRun = bMonitorRun;
}

void CServerLinker::closeSock(void)
{
    if (Q_INVALID_SOCK != m_Sock)
    {
        evutil_closesocket(m_Sock);
        m_Sock = Q_INVALID_SOCK;
    }
}

int CServerLinker::Link(void)
{
    closeSock();

    int iRtn = Q_RTN_OK;
    CNETAddr objAddr;
    int iKeepAlive = 1;

    iRtn = objAddr.setAddr(m_strDesIp.c_str(), m_usDesPort);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    m_Sock = socket(AF_INET, SOCK_STREAM, 0);
    if (Q_INVALID_SOCK == m_Sock)
    {
        iRtn = Q_SockError();
        Q_Printf("create socket error. error code %d, message %s", 
            iRtn, Q_SockError2Str(iRtn));

        return iRtn;
    }

    iRtn = connect(m_Sock, objAddr.getAddr(), objAddr.getAddrSize());
    if (Q_RTN_OK != iRtn)
    {
        iRtn = Q_SockError();
        Q_Printf("connect %s on port %d error. error code %d, message %s", 
            objAddr.getIp().c_str(), objAddr.getPort(), iRtn, Q_SockError2Str(iRtn));

        return iRtn;
    }

    (void)setsockopt(m_Sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, sizeof(iKeepAlive));

    return Q_RTN_OK;
}

int CServerLinker::addInLoop(void)
{
    int iRtn = Q_RTN_OK;
    OrderMsg stOrderMsg;
    
    stOrderMsg.emType = OrderType_AddServerLinker;
    stOrderMsg.pHandle = this;

    if (!getStop())
    {
        iRtn = m_pEvent->sendOrderMsg((const char*)(&stOrderMsg),  sizeof(stOrderMsg));
    }

    return iRtn;
}

int CServerLinker::Monitor(void)
{
    CServerLinkerMonitor *pMonitor = NULL;

    pMonitor = new(std::nothrow) CServerLinkerMonitor();
    if (NULL == pMonitor)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

        return Q_ERROR_ALLOCMEMORY;
    }

    pMonitor->setServerLinker(this);

    try
    {
        CThread objThread;

        //等待线程启动
        m_objMutex.Lock();
        objThread.Execute(pMonitor);
        m_objCond.Wait(&m_objMutex, 1000);
        m_objMutex.unLock();
    }
    catch (CException &e)
    {
        Q_SafeDelete(pMonitor);
        Q_Printf("exception code %d message %s", e.getErrorCode(), e.getErrorMsg());

        return e.getErrorCode();
    }

    return Q_RTN_OK;
}
