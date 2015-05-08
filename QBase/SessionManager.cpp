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

#include "SessionManager.h"
#include "WorkThreadEvent.h"

#define Q_INITSESSIONSIZE 2048

CSessionManager::CSessionManager(void) : m_uiTimer(Q_INIT_NUMBER),
    m_uiCount(Q_INIT_NUMBER), m_pLua(NULL), m_pCurrent(NULL), m_pInterface(NULL),
    m_pWorkThread(NULL) ,m_objWebSockParser(), m_objTcpParser(), m_objLinkOther(),
    m_objBinary()
{
    for (int i = 0; i < Q_INITSESSIONSIZE; i++)
    {
        CSession *pSession = new(std::nothrow) CSession();
        if (NULL == pSession)
        {
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

            break;
        }

        m_quFreeSession.push(pSession);
    }
}

CSessionManager::~CSessionManager(void)
{
    try
    {
        freeAllSession();
    }
    catch(...)
    {

    }    
}

void CSessionManager::setTimer(const unsigned int &uiMS)
{
    m_uiTimer = uiMS;
}

unsigned int CSessionManager::getTimer(void) const
{
    return m_uiTimer;
}

void CSessionManager::addCount(void)
{
    m_uiCount++;
}

unsigned int CSessionManager::getCount(void) const
{
    return m_uiCount;
}

void CSessionManager::setLua(struct lua_State *pLua)
{
    assert(NULL != pLua);

    m_pLua = pLua;
    m_objBinary.setLua(pLua);
}

void CSessionManager::setWorkThread(class CWorkThreadEvent *pThread)
{
    m_pWorkThread = pThread;
}

void CSessionManager::setCurSession(CSession *pSession)
{
    m_pCurrent = pSession;
}

void CSessionManager::dellSession(struct bufferevent *bev)
{
    std::tr1::unordered_map<int, CSession *>::iterator itSession;    
    evutil_socket_t fd = bufferevent_getfd(bev);

    itSession = m_umapSession.find(fd);
    if (m_umapSession.end() == itSession)
    {
        return;
    }

    CSession *pSession = itSession->second;
    if (NULL == pSession)
    {
        m_umapSession.erase(itSession);

        return;
    }

    pSession->Clear();
    m_quFreeSession.push(pSession);//加进空闲列表
    m_umapSession.erase(itSession);

    return;
}

int CSessionManager::addSession(struct bufferevent *bev)
{
    int iRtn = Q_RTN_OK;
    CSession *pSession = NULL;

    if (m_quFreeSession.empty())
    {
        pSession = new(std::nothrow) CSession();
        if (NULL == pSession)
        {
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

            return Q_ERROR_ALLOCMEMORY;
        }
    }
    else
    {
        pSession = m_quFreeSession.front();
        m_quFreeSession.pop();
    }
    
    iRtn = pSession->getBuffer()->setBuffer(bev);
    if (Q_RTN_OK != iRtn)
    {
        pSession->Clear();
        m_quFreeSession.push(pSession);

        return iRtn;
    }

    Q_SOCK fd = pSession->getBuffer()->getFD();
    pSession->setSessionID(fd);
    pSession->setPing(getCount());

    m_umapSession[fd] = pSession;

    return Q_RTN_OK;
}

CSession *CSessionManager::getSession(struct bufferevent *bev)
{
    evutil_socket_t fd = bufferevent_getfd(bev);
    if (Q_INVALID_SOCK == fd)
    {
        return NULL;
    }

    std::tr1::unordered_map<int, CSession *>::iterator itSession;
    itSession = m_umapSession.find(fd);
    if (m_umapSession.end() == itSession)
    {
        return NULL;
    }

    return itSession->second;
}

void CSessionManager::confirmStop(void)
{
    assert(NULL != m_pWorkThread);

    m_pWorkThread->setRunStatus(RunStatus_Stopped);
}

bool CSessionManager::addLinkOther(const char *pszIp, const unsigned short usPort, const char *pszNam)
{
    return m_objLinkOther.addHost(pszIp, usPort, pszNam);
}

int CSessionManager::getLinkOtherID(const char *pszName)
{
    assert(NULL != pszName);

    return m_objLinkOther.getSockByName(pszName);
}

size_t CSessionManager::getSessionSize(void) const
{
    return m_umapSession.size();
}

void CSessionManager::freeAllSession(void)
{
    std::tr1::unordered_map<int, CSession *>::iterator itSession;
    CSession *pSession = NULL;

    for (itSession = m_umapSession.begin(); m_umapSession.end() != itSession; itSession++)
    {
        bufferevent_free(itSession->second->getBuffer()->getBuffer());
        Q_SafeDelete(itSession->second);
    }

    while(!m_quFreeSession.empty())
    {
        pSession = m_quFreeSession.front();
        m_quFreeSession.pop();
        Q_SafeDelete(pSession);
    }

    m_umapSession.clear();

    return;
}

void CSessionManager::closeLinkByID(const int iID)
{
    std::tr1::unordered_map<int, CSession *>::iterator itSession;
    itSession = m_umapSession.find(iID);
    if (m_umapSession.end() == itSession)
    {
        return;
    }

    m_pInterface->onSockClose(itSession->second);

    switch(itSession->second->getType())
    {
    case STYPE_WEBSOCK:
        {
            //外面已经清理 CWorkThreadEvent::dispWebSock
        }
        break;

    case STYPE_TCPCLIENT:
        {
            getLinkOther()->setSockStatus(iID, SessionStatus_Closed);
        }
        break;

    default:
        break;
    }

    bufferevent_free(itSession->second->getBuffer()->getBuffer());
    itSession->second->Clear();

    m_quFreeSession.push(itSession->second);
    m_umapSession.erase(itSession);

    return;
}

void CSessionManager::closeCurLink(void)
{
    if (NULL == m_pCurrent)
    {
        return;
    }

    closeLinkByID(m_pCurrent->getSessionID());
}

void CSessionManager::setInterface(class CEventInterface *pInterface)
{
    pInterface->setSessionManager(this);
    m_pInterface = pInterface;
}

class CEventInterface *CSessionManager::getInterface(void)
{
    return m_pInterface;
}

CSession *CSessionManager::getCurSession(void)
{
    return m_pCurrent;
}

CSession *CSessionManager::getSessionByID(const int iID)
{
    std::tr1::unordered_map<int, CSession *>::iterator itSession;

    itSession = m_umapSession.find(iID);
    if (m_umapSession.end() == itSession)
    {
        return NULL;
    }

    return itSession->second;
}

bool CSessionManager::sendWebSock(CSession *pCurrent, const char *pszData, const size_t &uiLens)
{
    size_t iOutLens = Q_INIT_NUMBER;
    const char *pszHead = m_objWebSockParser.createHead(true, TEXT_FRAME, uiLens, iOutLens);

    if (Q_RTN_OK != pCurrent->getBuffer()->writeBuffer(pszHead, iOutLens))
    {
        Q_Printf("send message to session: id %d error", pCurrent->getSessionID());

        return false;
    }

    if ((NULL != pszData) 
        && (0 != uiLens))
    {
        if (Q_RTN_OK != pCurrent->getBuffer()->writeBuffer(pszData, uiLens))
        {
            Q_Printf("send message to session: id %d error", pCurrent->getSessionID());

            return false;
        }
    }

    return true;
}

bool CSessionManager::sendTcp(CSession *pCurrent, const char *pszData, const size_t &uiLens)
{
    size_t iHeadLens = Q_INIT_NUMBER;
    const char *pszHead = m_objTcpParser.createHead(uiLens, iHeadLens);

    if (Q_RTN_OK != pCurrent->getBuffer()->writeBuffer(pszHead, iHeadLens))
    {
        Q_Printf("send message lens to session: id %d error", pCurrent->getSessionID());

        return false;
    }

    if ((NULL != pszData) 
        && (0 != uiLens))
    {
        if (Q_RTN_OK != pCurrent->getBuffer()->writeBuffer(pszData, uiLens))
        {
            Q_Printf("send message to session: id %d error", pCurrent->getSessionID());

            return false;
        }
    }

    return true;
}

bool CSessionManager::sendToCur(const char *pszData, const size_t uiLens)
{
    if (NULL == m_pCurrent)
    {
        Q_Printf("%s", "current session pointer is null.");
        return false;
    }

    bool bOK = false;
    switch(m_pCurrent->getType())
    {
    case STYPE_TCPCLIENT:
    case STYPE_TCP:
        {
            bOK = sendTcp(m_pCurrent, pszData, uiLens);
        }
        break;

    case STYPE_WEBSOCK:
        {
            bOK = sendWebSock(m_pCurrent, pszData, uiLens);
        }
        break;

    default:
        break;
    }

    return bOK;
}

bool CSessionManager::sendToByID(const int iID, const char *pszData, const size_t uiLens)
{
    std::tr1::unordered_map<int, CSession *>::iterator itSession;

    itSession = m_umapSession.find(iID);
    if (m_umapSession.end() == itSession)
    {
        Q_Printf("no find id %u", iID);
        return false;
    }

    if (NULL == itSession->second)
    {
        Q_Printf("%s", "input session pointer is null.");

        return false;
    }

    bool bOK = false;
    switch(itSession->second->getType())
    {
    case STYPE_TCPCLIENT:
    case STYPE_TCP:
        {
            bOK = sendTcp(itSession->second, pszData, uiLens);
        }
        break;

    case STYPE_WEBSOCK:
        {
            bOK = sendWebSock(itSession->second, pszData, uiLens);
        }
        break;

    default:
        break;
    }

    return bOK;
}

bool CSessionManager::sendBToCur(void)
{
    return sendToCur(m_objBinary.getBufferMgr()->getBuffer(), m_objBinary.getBufferMgr()->getLens());
}

bool CSessionManager::sendBToByID(const int iID)
{
    return sendToByID(iID, m_objBinary.getBufferMgr()->getBuffer(), m_objBinary.getBufferMgr()->getLens());
}
