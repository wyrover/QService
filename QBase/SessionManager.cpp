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
#include "Exception.h"
#include "ServerLinker.h"

#define Q_INITSESSIONSIZE 2000

CSessionManager::CSessionManager(void) : m_sThreadIndex(-1), m_uiTimer(Q_INIT_NUMBER),
    m_uiCount(Q_INIT_NUMBER), m_pLua(NULL), m_pCurrent(NULL), m_pInterface(NULL)
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
    freeAllSession();
}

void CSessionManager::setTimer(unsigned int &uiMS)
{
    m_uiTimer = uiMS;
}

unsigned int CSessionManager::getTimer(void)
{
    return m_uiTimer;
}

void CSessionManager::addCount(void)
{
    m_uiCount++;
}

unsigned int CSessionManager::getCount(void)
{
    return m_uiCount;
}

void CSessionManager::setLua(struct lua_State *pLua)
{
    m_pLua = pLua;
}

void CSessionManager::setThreadIndex(const short &sIndex)
{
    m_sThreadIndex = sIndex; 
}

short CSessionManager::getThreadIndex(void)
{
    return m_sThreadIndex;
}

void CSessionManager::setCurSession(CSession *pSession)
{
    m_pCurrent = pSession;
}

void CSessionManager::dellSession(struct bufferevent *bev)
{
    std::tr1::unordered_map<int, CSession *>::iterator itSession;    
    evutil_socket_t fd = bufferevent_getfd(bev);

    itSession = m_unmapSession.find(fd);
    if (m_unmapSession.end() == itSession)
    {
        return;
    }

    CSession *pSession = itSession->second;
    if (NULL == pSession)
    {
        m_unmapSession.erase(itSession);

        return;
    }

    pSession->Clear();
    m_quFreeSession.push(pSession);//加进空闲列表
    m_unmapSession.erase(itSession);

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

    m_unmapSession[fd] = pSession;

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
    itSession = m_unmapSession.find(fd);
    if (m_unmapSession.end() == itSession)
    {
        return NULL;
    }

    return itSession->second;
}

void CSessionManager::addServerLinker(const char *pszName, struct bufferevent *pBufEvent)
{
    std::tr1::unordered_map<std::string, bufferevent* >::iterator itServerLinker;
    std::string strName(pszName);

    itServerLinker = m_mapServerLinker.find(strName);
    if (m_mapServerLinker.end() == itServerLinker)
    {
        m_mapServerLinker.insert(std::make_pair(strName, pBufEvent));
    }
    else
    {
        itServerLinker->second = pBufEvent;
    }
}

void CSessionManager::delServerLinker(const char *pszName)
{
    std::tr1::unordered_map<std::string, bufferevent* >::iterator itServerLinker;

    itServerLinker = m_mapServerLinker.find(std::string(pszName));
    if (m_mapServerLinker.end() != itServerLinker)
    {
        m_mapServerLinker.erase(itServerLinker);
    }
}

CSession *CSessionManager::getServerLinkerSession(const char *pszName)
{
    std::tr1::unordered_map<std::string, bufferevent* >::iterator itServerLinker;

    itServerLinker = m_mapServerLinker.find(std::string(pszName));
    if (m_mapServerLinker.end() == itServerLinker)
    {
        return NULL;
    }

    return getSession(itServerLinker->second);
}

size_t CSessionManager::getSessionSize(void)
{
    return m_unmapSession.size();
}

void CSessionManager::freeAllSession(void)
{
    std::tr1::unordered_map<int, CSession *>::iterator itSession;
    CSession *pSession = NULL;

    for (itSession = m_unmapSession.begin(); m_unmapSession.end() != itSession; itSession++)
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

    m_unmapSession.clear();

    return;
}

void CSessionManager::closeLinkByID(const int iID)
{
    std::tr1::unordered_map<int, CSession *>::iterator itSession;

    itSession = m_unmapSession.find(iID);
    if (m_unmapSession.end() == itSession)
    {
        return;
    }

    setCurSession(itSession->second);
    m_pInterface->onSocketClose();
    setCurSession(NULL);

    bufferevent_free(itSession->second->getBuffer()->getBuffer());
    itSession->second->Clear();

    m_quFreeSession.push(itSession->second);
    m_unmapSession.erase(itSession);

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

    itSession = m_unmapSession.find(iID);
    if (m_unmapSession.end() == itSession)
    {
        return NULL;
    }

    return itSession->second;
}

bool CSessionManager::checkParam(CSession *pCurrent, 
    const char *pszData, const size_t &uiLens)
{
    if (NULL == pCurrent)
    {
        Q_Printf("%s", "input session pointer is null.");
        return false;
    }
    if (NULL == pszData)
    {
        Q_Printf("%s", "input data pointer is null.");
        return false;
    }
    if (0 >= uiLens)
    {
        Q_Printf("%s", "message lens must big than zero.");
        return false;
    }

    return true;
}

bool CSessionManager::sendWithHead(CSession *pCurrent, 
    const char *pszData, const size_t &uiLens)
{
    Q_PackHeadType msgLens = Q_NTOH(uiLens);

    if (Q_RTN_OK != pCurrent->getBuffer()->writeBuffer((const char*)(&msgLens), sizeof(msgLens)))
    {
        Q_Printf("send message lens to session: id %d error", pCurrent->getSessionID());

        return false;
    }

    if (Q_RTN_OK != pCurrent->getBuffer()->writeBuffer(pszData, uiLens))
    {
        Q_Printf("send message to session: id %d error", pCurrent->getSessionID());

        return false;
    }

    return true;
}

bool CSessionManager::sendToCur(const char *pszData, const size_t uiLens)
{
    if (!checkParam(m_pCurrent, pszData, uiLens))
    {
        return false;
    }

    return sendWithHead(m_pCurrent, pszData, uiLens);
}

bool CSessionManager::sendToByID(const int iID, const char *pszData, const size_t uiLens)
{
    std::tr1::unordered_map<int, CSession *>::iterator itSession;

    itSession = m_unmapSession.find(iID);
    if (m_unmapSession.end() == itSession)
    {
        Q_Printf("no find id %u", iID);
        return false;
    }

    if (!checkParam(itSession->second, pszData, uiLens))
    {
        return false;
    }

    return sendWithHead(itSession->second, pszData, uiLens);
}

bool CSessionManager::sendToAll(const char *pszData, const size_t uiLens)
{
    if (NULL == pszData)
    {
        Q_Printf("%s", "input data pointer is null.");
        return false;
    }
    if (0 >= uiLens)
    {
        Q_Printf("%s", "message lens must big than zero.");
        return false;
    }

    std::tr1::unordered_map<int, CSession *>::iterator itSession;
    for (itSession = m_unmapSession.begin(); m_unmapSession.end() != itSession; itSession++)
    {
        if (itSession->second->getServerLinker())
        {
            continue;
        }

        (void)sendWithHead(itSession->second, pszData, uiLens);
    }

    return true;
}

bool CSessionManager::pushMsg(const char *pszData, const size_t usLens)
{
    try
    {
        m_objBuffer.pushBuff(pszData, usLens);
    }
    catch (CException &e)
    {
        Q_Printf("%s", e.getErrorMsg());

        return false;
    }

    return true;
}

bool CSessionManager::sendPushMsgToCur(void)
{
    return sendToCur(m_objBuffer.getBuffer(), m_objBuffer.getLens());
}

bool CSessionManager::sendPushMsgToByID(const int iID)
{
    return sendToByID(iID, m_objBuffer.getBuffer(), m_objBuffer.getLens());
}

bool CSessionManager::sendPushMsgToAll(void)
{
    return sendToAll(m_objBuffer.getBuffer(), m_objBuffer.getLens());
}

void CSessionManager::Clear(void)
{
    m_objBuffer.reSet();
}

luabridge::LuaRef CSessionManager::getSVLinkerNameByType(const int iType)
{
    assert(NULL != m_pLua);

    CServerLinker *pLinker = NULL;
    CSession *pSession = NULL;
    luabridge::LuaRef luaTable = luabridge::newTable(m_pLua);
    std::tr1::unordered_map<std::string, bufferevent* >::iterator itServerLinker;

    for (itServerLinker = m_mapServerLinker.begin(); m_mapServerLinker.end() != itServerLinker; 
        itServerLinker++)
    {
        pSession = getSession(itServerLinker->second);
        if (pSession->getServerLinker())
        {
            pLinker = (CServerLinker *)(pSession->getHandle());
            if (NULL != pLinker)
            {
                if (iType == pLinker->getType())
                {
                    luaTable.append(pLinker->getLinkerName());
                }
            }
        }
    }

    return luaTable;
}

bool CSessionManager::checkType(const int iType, const int iClientID)
{
    CServerLinker *pLinker = NULL;
    CSession *pSesson = getSessionByID(iClientID);
    if(NULL == pSesson)
    {
        return false;
    }

    if (!(pSesson->getServerLinker()))
    {
        return false;
    }

    pLinker = (CServerLinker *)(pSesson->getHandle());
    if (NULL == pLinker)
    {
        return false;
    }

    return (pLinker->getType() == iType) ? true : false;
}

int CSessionManager::getGetSVLinkerNum(void)
{
    return m_mapServerLinker.size();
}
