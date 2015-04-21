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

#include "LinkOther.h"
#include "WorkThreadEvent.h"
#include "Thread.h"

class CLinkOtherTask : public CTask
{
public:
    void setHandle(CLinkOther *pLinker)
    {
        m_pLinker = pLinker;
    };
    void Run(void)
    {
        m_pLinker->setStatus(RunStatus_Runing);
        unsigned long long ullCount = Q_INIT_NUMBER;

        while(RunStatus_Runing == m_pLinker->getStatus())
        {
            if (0 == (ullCount % 10))
            {
                m_pLinker->Link();
            }            

            Q_Sleep(100);
            ullCount++;
        }

        m_pLinker->setStatus(RunStatus_Stopped);
    };
private:
    CLinkOther *m_pLinker;
};

CLinkOther::CLinkOther(void) : m_emStatus(RunStatus_Unknown), m_pThreadEvent(NULL)
{

}

CLinkOther::~CLinkOther(void)
{
    Stop();
    std::vector<LinkInfo>::iterator itInfo;
    for (itInfo = m_vcLinkInfo.begin(); m_vcLinkInfo.end() != itInfo; itInfo++)
    {
        if (Q_INVALID_SOCK != itInfo->sock)
        {
            evutil_closesocket(itInfo->sock);
            itInfo->sock = Q_INVALID_SOCK;
        }        
    }
    m_vcLinkInfo.clear();
}

void CLinkOther::setThreadEvent(class CWorkThreadEvent *pThreadEvent)
{
    m_pThreadEvent = pThreadEvent;
}

class CWorkThreadEvent *CLinkOther::getThreadEvent(void)
{
    return m_pThreadEvent;
}

void CLinkOther::setStatus(const RunStatus emStatus)
{
    m_emStatus = emStatus;
}

RunStatus CLinkOther::getStatus(void)
{
    return m_emStatus;
}

bool CLinkOther::addHost(const char *pszIp, unsigned short usPort, const char *pszName)
{
    std::vector<LinkInfo>::iterator itInfo;
    for (itInfo = m_vcLinkInfo.begin(); m_vcLinkInfo.end() != itInfo; itInfo++)
    {
        if (std::string(pszName) == itInfo->strName)
        {
            Q_Printf("link other name %s already exist.",  pszName);
            return false;
        }
    }

    LinkInfo stLinkInfo;
    stLinkInfo.emStatus = SessionStatus_Closed;
    stLinkInfo.sock = Q_INVALID_SOCK;
    stLinkInfo.strIp = pszIp;
    stLinkInfo.strName = pszName;
    stLinkInfo.usPort = usPort;

    m_vcLinkInfo.push_back(stLinkInfo);

    return true;
}

bool CLinkOther::Start(void)
{
    CThread objThread;
    CLinkOtherTask *pTask = new(std::nothrow)CLinkOtherTask(); 
    if (NULL == pTask)
    {
        return false;
    }

    pTask->setHandle(this);

    objThread.Execute(pTask);

    return true;
}

void CLinkOther::Stop(void)
{
    if (RunStatus_Runing == getStatus())
    {
        setStatus(RunStatus_Stopping);
        while(true)
        {
            if (RunStatus_Stopped == getStatus())
            {
                break;
            }

            Q_Sleep(10);
        }
    }   
}

Q_SOCK CLinkOther::initSock(const char *pszIp, unsigned short usPort)
{
    CNETAddr objAddr;
    int iKeepAlive = 1;
    Q_SOCK sock = Q_INVALID_SOCK;

    if (Q_RTN_OK != objAddr.setAddr(pszIp, usPort))
    {
        return sock;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (Q_INVALID_SOCK == sock)
    {
        return sock;
    }

    if (Q_RTN_OK != connect(sock, objAddr.getAddr(), objAddr.getAddrSize()))
    {
        evutil_closesocket(sock);
        return Q_INVALID_SOCK;
    }

    (void)setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, sizeof(iKeepAlive));
    (void)evutil_make_socket_nonblocking(sock);

    return sock;
}

void CLinkOther::Link(void)
{
    Q_SOCK sok = Q_INVALID_SOCK;
    TriggerSock stTriggerSock;
    std::vector<LinkInfo>::iterator itInfo;

    for (itInfo = m_vcLinkInfo.begin(); m_vcLinkInfo.end() != itInfo; itInfo++)
    {
        if (SessionStatus_Closed == itInfo->emStatus)
        {
            Q_Printf("try link %s on port %d...", itInfo->strIp.c_str(), itInfo->usPort);
            if (Q_INVALID_SOCK != itInfo->sock)
            {
                evutil_closesocket(itInfo->sock);
                itInfo->sock = Q_INVALID_SOCK;
            }

            sok = initSock(itInfo->strIp.c_str(), itInfo->usPort);
            if (Q_INVALID_SOCK == sok)
            {
                continue;
            }

            stTriggerSock.emType = STYPE_TCPCLIENT;
            stTriggerSock.iSock = sok;
            if (Q_RTN_OK != m_pThreadEvent->sendAssistMsg((const char *)&stTriggerSock, sizeof(stTriggerSock)))
            {
                evutil_closesocket(sok);
            }
            else
            {
                itInfo->emStatus = SessionStatus_Connect;
                itInfo->sock = sok;
            }
        }
    }
}

void CLinkOther::setSockStatus(Q_SOCK sock, SessionStatus emStatus)
{
    if (Q_INVALID_SOCK == sock)
    {
        return;
    }

    std::vector<LinkInfo>::iterator itInfo;
    for (itInfo = m_vcLinkInfo.begin(); m_vcLinkInfo.end() != itInfo; itInfo++)
    {
        if (itInfo->sock == sock)
        {
            itInfo->emStatus = emStatus;
            break;
        }
    }
}

int CLinkOther::getSockByName(const char *pszName)
{
    if (NULL == pszName
        || 0 == strlen(pszName))
    {
        return Q_INVALID_SOCK;
    }

    std::vector<LinkInfo>::iterator itInfo;
    for (itInfo = m_vcLinkInfo.begin(); m_vcLinkInfo.end() != itInfo; itInfo++)
    {
        if (itInfo->strName == std::string(pszName))
        {
            return itInfo->sock;
        }
    }

    return Q_INVALID_SOCK;
}
