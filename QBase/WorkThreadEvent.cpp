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

#include "WorkThreadEvent.h"

#define Http_TimeOut  10

CWorkThreadEvent::CWorkThreadEvent(void) : m_pEvent(NULL)
{
    m_objSessionManager.setWorkThread(this);
}

CWorkThreadEvent::~CWorkThreadEvent(void)
{
    if (NULL != m_pEvent)
    {
        event_free(m_pEvent);
        m_pEvent = NULL;
    }

    std::vector<struct evhttp *>::iterator itHttp;
    for (itHttp = m_vcEvHttp.begin(); m_vcEvHttp.end() != itHttp; itHttp++)
    {
        evhttp_free(*itHttp);
    }
    m_vcEvHttp.clear();
}

CSessionManager *CWorkThreadEvent::getSessionManager(void)
{
    return &m_objSessionManager;
}

CWebSockParser *CWorkThreadEvent::getWebSockParser(void)
{
    return &m_objWebSockParser;
}

CTcpParser *CWorkThreadEvent::getTcpParser(void)
{
    return &m_objTcpParser;
}

CLinkOther *CWorkThreadEvent::getLinkOther(void)
{
    return m_objSessionManager.getLinkOther();
}

CHttpParser *CWorkThreadEvent::getHttpParser(void)
{
    return &m_objHttpParser;
}

bool CWorkThreadEvent::onStartUp(void)
{
    return m_objSessionManager.getInterface()->onSerciveStartUp();
}

void CWorkThreadEvent::setInterface(CEventInterface *pInterface)
{
    m_objSessionManager.setInterface(pInterface);
    pInterface->setSessionManager(&m_objSessionManager);
}

int CWorkThreadEvent::setTimer(unsigned int uiMS)
{
    if (Q_INIT_NUMBER == uiMS)
    {
        return Q_RTN_OK;
    }

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

    m_pEvent = event_new(getBase(), 
        -1, EV_PERSIST, workThreadTimerCB, this);
    if (NULL == m_pEvent)
    {
        Q_Printf("%s", "event_new error");

        return Q_RTN_FAILE;
    }

    if (Q_RTN_OK != event_add(m_pEvent, &tVal))
    {
        Q_Printf("%s", "event_add error");

        event_free(m_pEvent);
        m_pEvent = NULL;

        return Q_RTN_FAILE;
    }

    m_objSessionManager.setTimer(uiMS);

    return Q_RTN_OK;
}

int CWorkThreadEvent::setHttpSock(std::vector<Q_SOCK> &vcHttpSock)
{
    std::vector<Q_SOCK>::iterator itSock;
    for (itSock = vcHttpSock.begin(); vcHttpSock.end() != itSock; itSock++)
    {
        struct evhttp *pHttp = evhttp_new(getBase());
        if (NULL == pHttp)
        {
            return Q_RTN_FAILE;
        }

        if (Q_RTN_OK != evhttp_accept_socket(pHttp, *itSock))
        {
            evhttp_free(pHttp);
            return Q_RTN_FAILE;
        }

        evhttp_set_timeout(pHttp, Http_TimeOut);
        evhttp_set_gencb(pHttp, workThreadHttpCB, this);

        m_vcEvHttp.push_back(pHttp);
    }

    return Q_RTN_OK;
}

void CWorkThreadEvent::onMainRead(CEventBuffer *pBuffer)
{
    struct bufferevent *pBev = NULL;
    CSession *pSession = NULL;

    while(Q_GetEventValue<TriggerSock>(getMainBuffer(), m_stWorkSock))
    {
        (void)evutil_make_socket_nonblocking(m_stWorkSock.iSock);
        pBev = bufferevent_socket_new(getBase(), m_stWorkSock.iSock, 
            BEV_OPT_CLOSE_ON_FREE);
        if (NULL == pBev)
        {
            evutil_closesocket(m_stWorkSock.iSock);
            Q_Printf("%s", "bufferevent_socket_new error.");

            continue;
        }

        if (Q_RTN_OK != m_objSessionManager.addSession(pBev))
        {
            evutil_closesocket(m_stWorkSock.iSock);
            bufferevent_free(pBev);
            Q_Printf("%s", "add session error.");

            continue;
        }

        bufferevent_setcb(pBev, workThreadReadCB, NULL, workThreadEventCB, 
            this);
        if (Q_RTN_OK != bufferevent_enable(pBev, EV_READ | EV_WRITE))
        {
            evutil_closesocket(m_stWorkSock.iSock);
            m_objSessionManager.dellSession(pBev);
            bufferevent_free(pBev);

            Q_Printf("%s", "bufferevent_enable error.");

            continue;
        }

        pSession = m_objSessionManager.getSession(pBev);
        pSession->setType(m_stWorkSock.emType);
        pSession->setStatus(SessionStatus_Linked);
        if (STYPE_WEBSOCK == m_stWorkSock.emType)
        {
            pSession->setStatus(SessionStatus_Connect);
        }
    }
}

void CWorkThreadEvent::onAssistRead(CEventBuffer *pBuffer)
{
    struct bufferevent *pBev = NULL;
    CSession *pSession = NULL;

    while(Q_GetEventValue<TriggerSock>(getAssistBuffer(), m_stWorkSock))
    {
        (void)evutil_make_socket_nonblocking(m_stWorkSock.iSock);
        pBev = bufferevent_socket_new(getBase(), m_stWorkSock.iSock, 
            BEV_OPT_CLOSE_ON_FREE);
        if (NULL == pBev)
        {
            m_objSessionManager.getLinkOther()->setSockStatus(m_stWorkSock.iSock, SessionStatus_Closed);            
            Q_Printf("%s", "bufferevent_socket_new error.");

            continue;
        }

        if (Q_RTN_OK != m_objSessionManager.addSession(pBev))
        {
            m_objSessionManager.getLinkOther()->setSockStatus(m_stWorkSock.iSock, SessionStatus_Closed);
            bufferevent_free(pBev);
            Q_Printf("%s", "add session error.");

            continue;
        }

        bufferevent_setcb(pBev, workThreadReadCB, NULL, workThreadEventCB, 
            this);
        if (Q_RTN_OK != bufferevent_enable(pBev, EV_READ | EV_WRITE))
        {
            m_objSessionManager.getLinkOther()->setSockStatus(m_stWorkSock.iSock, SessionStatus_Closed);
            m_objSessionManager.dellSession(pBev);
            bufferevent_free(pBev);

            Q_Printf("%s", "bufferevent_enable error.");

            continue;
        }

        pSession = m_objSessionManager.getSession(pBev);
        pSession->setType(m_stWorkSock.emType);
        pSession->setStatus(SessionStatus_Linked);
        m_objSessionManager.getLinkOther()->setSockStatus(m_stWorkSock.iSock, SessionStatus_Linked);

        m_objSessionManager.getInterface()->onLinkedOther(pSession);
    }
}

void CWorkThreadEvent::onStop(void)
{
    m_objSessionManager.getInterface()->onSerciveShutDown();    
}

void CWorkThreadEvent::dispTcp(CWorkThreadEvent *pWorkThreadEvent,
    CSessionManager *pSessionManager, CSession *pSession)
{
    CTcpParser *pParser = pWorkThreadEvent->getTcpParser();

    while(true)
    {
        const char *pszBuf = pParser->parsePack(pSession->getBuffer());
        if (NULL == pszBuf)
        {
            return;
        }

        pSessionManager->getInterface()->onTcpRead(pszBuf, pParser->getBufLens());
        if (SessionStatus_Closed != pSession->getStatus())
        {
            (void)pSession->getBuffer()->delBuffer(pParser->getParsedLens());
        }
        else
        {
            return;
        }
    }
}

void CWorkThreadEvent::dispWebSock(CWorkThreadEvent *pWorkThreadEvent,
    CSessionManager *pSessionManager, CSession *pSession)
{
    CWebSockParser *pParser = pWorkThreadEvent->getWebSockParser();

    //握手处理
    if (SessionStatus_Connect == pSession->getStatus())
    {
        std::string *pStrShakeHandsRtn = pParser->shakeHands(pSession->getBuffer());
        if (pParser->getClose())
        {
            pSessionManager->closeLinkByID(pSession->getSessionID());

            return;
        }
        if (NULL == pStrShakeHandsRtn)
        {
            return;
        }

        (void)pSession->getBuffer()->writeBuffer(pStrShakeHandsRtn->c_str(), pStrShakeHandsRtn->size());
        pSession->setStatus(SessionStatus_Linked);

        (void)pSession->getBuffer()->delBuffer(pParser->getParsedLens());

        return;
    }
    
    //正常数据处理
    bool bOk = false;
    WebSockFram *pHead = NULL;
    bool bControl = false;
    while(true)
    {
        //解包
        bOk = pParser->parsePack(pSession->getBuffer());
        if (pParser->getClose())
        {
            pWorkThreadEvent->delContinuation(pSession->getBuffer()->getFD());
            pSessionManager->closeLinkByID(pSession->getSessionID());

            return;
        }
        if (!bOk)
        {
            return;
        }

        pHead = pParser->getHead();

        //控制帧
        bControl = false;
        switch(pHead->emOpCode)
        {
        case CLOSE:
            {
                pWorkThreadEvent->delContinuation(pSession->getBuffer()->getFD());
                pSessionManager->closeLinkByID(pSession->getSessionID());

                return;
            }
            break;
        case PING:
            {
                bControl = true;
                size_t iOutLens = Q_INIT_NUMBER;
                const char *pHead = pParser->createHead(true, PONG, 0, iOutLens);

                pSession->getBuffer()->writeBuffer(pHead, iOutLens);
                pSession->setPing(pSessionManager->getCount());
            }
            break;
        case PONG:
            {
                bControl = true;
                pSession->setPing(pSessionManager->getCount());
            }
            break;
        default:
            break;
        }

        if (bControl)
        {
            (void)pSession->getBuffer()->delBuffer(pParser->getParsedLens());

            continue;
        }

        Q_SOCK sock = pSession->getBuffer()->getFD();

        //完整帧
        if ((CONTINUATION != pHead->emOpCode)
            && (1 == pHead->cFin))
        {
            pSessionManager->getInterface()->onWebSockRead(pParser->getMsg(), pHead->uiDataLens);
            if (SessionStatus_Closed == pSession->getStatus())
            {
                pWorkThreadEvent->delContinuation(sock);
                return;
            }
            else
            {
                (void)pSession->getBuffer()->delBuffer(pParser->getParsedLens());
                continue;
            }
        }

        /*一个分片的消息由起:
        始帧（FIN为0，opcode非0），
        若干（0个或多个）帧（FIN为0，opcode为0），
        结束帧（FIN为1，opcode为0）*/
        //是否为结束帧
        if ((1 == pHead->cFin)
            && (CONTINUATION == pHead->emOpCode))
        {
            std::string *pStrBuf = pWorkThreadEvent->getContinuation(sock);
            if (NULL != pStrBuf)
            {
                pStrBuf->append(pParser->getMsg(), pHead->uiDataLens);
                pSessionManager->getInterface()->onWebSockRead(pStrBuf->c_str(), pStrBuf->size());
                if (SessionStatus_Closed == pSession->getStatus())
                {
                    pWorkThreadEvent->delContinuation(sock);
                    return;
                }
                else
                {
                    (void)pSession->getBuffer()->delBuffer(pParser->getParsedLens());
                    pStrBuf->clear();
                }
            }
            else
            {
                //丢弃该包
                (void)pSession->getBuffer()->delBuffer(pParser->getParsedLens());
            }
        }
        else
        {
            pWorkThreadEvent->addContinuation(sock, pParser->getMsg(), pHead->uiDataLens);
            (void)pSession->getBuffer()->delBuffer(pParser->getParsedLens());
        }
    }

    return;
}

void CWorkThreadEvent::workThreadReadCB(struct bufferevent *bev, void *arg)
{ 
    CWorkThreadEvent *pWorkThreadEvent = (CWorkThreadEvent *)arg;
    CSessionManager *pSessionManager = pWorkThreadEvent->getSessionManager();
    CSession *pSession = pSessionManager->getSession(bev);
    if (NULL == pSession)
    {
        Q_Printf("%s", "get session error.");
        bufferevent_free(bev);

        return;
    }

    pSessionManager->setCurSession(pSession);
    switch(pSession->getType())
    {
    case STYPE_TCPCLIENT:
    case STYPE_TCP:
        {
            dispTcp(pWorkThreadEvent, pSessionManager, pSession);
        }
        break;

    case STYPE_WEBSOCK:
        {
            dispWebSock(pWorkThreadEvent, pSessionManager, pSession);
        }
        break;

    default:
        break;
    }    
    pSessionManager->setCurSession(NULL);
}

void CWorkThreadEvent::workThreadEventCB(struct bufferevent *bev, short event, void *arg)
{
    CWorkThreadEvent *pWorkThreadEvent = (CWorkThreadEvent *)arg;
    CSessionManager *pSessionManager = pWorkThreadEvent->getSessionManager();
    CSession *pSession = pSessionManager->getSession(bev);
    if (NULL == pSession)
    {
        bufferevent_free(bev);

        return;
    }

    pSessionManager->setCurSession(pSession);
    pSessionManager->getInterface()->onSocketClose();
    pSessionManager->setCurSession(NULL);

    switch(pSession->getType())
    {
    case STYPE_WEBSOCK:
        {
            pWorkThreadEvent->delContinuation(pSession->getBuffer()->getFD());
        }
        break;

    case STYPE_TCPCLIENT:
        {
            pSessionManager->getLinkOther()->setSockStatus(pSession->getBuffer()->getFD(), 
                SessionStatus_Closed);
        }
        break;

    default:
        break;
    }
    
    pSessionManager->dellSession(bev);
    bufferevent_free(bev);
}

void CWorkThreadEvent::workThreadTimerCB(evutil_socket_t, short event, void *arg)
{
    CWorkThreadEvent *pWorkThreadEvent = (CWorkThreadEvent *)arg;
    CSessionManager *pSessionManager = pWorkThreadEvent->getSessionManager();

    pSessionManager->addCount();

    pSessionManager->getInterface()->onTimerEvent();
}

void CWorkThreadEvent::workThreadHttpCB(struct evhttp_request *req, void *arg)
{
    CWorkThreadEvent *pWorkThreadEvent = (CWorkThreadEvent *)arg;
    CSessionManager *pSessionManager = pWorkThreadEvent->getSessionManager();
    CHttpParser *pHttpParser = pWorkThreadEvent->getHttpParser();
    if (pHttpParser->setHttpRequest(req))
    {
        pSessionManager->getInterface()->onHttpRead(pHttpParser);
    }
}

void CWorkThreadEvent::addContinuation(const Q_SOCK &sock, const char *pszData, const size_t &iLens)
{
    std::tr1::unordered_map<Q_SOCK, std::string>::iterator itFram;
    itFram = m_mapWebSockPack.find(sock);
    if (m_mapWebSockPack.end() == itFram)
    {
        std::string strBuf(pszData, iLens);
        m_mapWebSockPack.insert(std::make_pair(sock, strBuf));

        return;
    }

    itFram->second.append(pszData, iLens);
}

std::string *CWorkThreadEvent::getContinuation(const Q_SOCK &sock)
{
    std::tr1::unordered_map<Q_SOCK, std::string>::iterator itFram;
    itFram = m_mapWebSockPack.find(sock);
    if (m_mapWebSockPack.end() == itFram)
    {
        return NULL;
    }

    return &(itFram->second);
}

void CWorkThreadEvent::delContinuation(const Q_SOCK &sock)
{
    std::tr1::unordered_map<Q_SOCK, std::string>::iterator itFram;
    itFram = m_mapWebSockPack.find(sock);
    if (m_mapWebSockPack.end() == itFram)
    {
        return;
    }

    m_mapWebSockPack.erase(itFram);
}
