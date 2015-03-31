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
#include "SockPairEventParam.h"
#include "HttpBuffer.h"

#define Http_TimeOut  60

CWorkThreadEvent::CWorkThreadEvent(void) : m_pEvent(NULL), m_pHttp(NULL)
{
    setTcpParam(&m_objSessionManager);
    setExitParam(&m_objSessionManager);
    setOrderParam(&m_objSessionManager);
    setWebSockParam(&m_objSessionManager);
}

CWorkThreadEvent::~CWorkThreadEvent(void)
{
    Stop();
    if (NULL != m_pEvent)
    {
        event_free(m_pEvent);
        m_pEvent = NULL;
    }
    if (NULL != m_pHttp)
    {
        evhttp_free(m_pHttp);
        m_pHttp = NULL;
    }
}

CSessionManager *CWorkThreadEvent::getSessionManager(void)
{
    return &m_objSessionManager;
}

void CWorkThreadEvent::onStartUp(void)
{
    m_objSessionManager.getInterface()->onSerciveStartUp();
}

void CWorkThreadEvent::setInterface(CEventInterface *pInterface)
{
    pInterface->setSessionManager(&m_objSessionManager);
    m_objSessionManager.setInterface(pInterface);
}

int CWorkThreadEvent::setHttp(Q_SOCK &sock)
{
    if (Q_INVALID_SOCK == sock)
    {
        return Q_RTN_OK;
    }
    
    m_pHttp = evhttp_new(getBase());
    if (NULL == m_pHttp)
    {
        return Q_RTN_FAILE;
    }
    
    if (Q_RTN_OK != evhttp_accept_socket(m_pHttp, sock))
    {
        return Q_RTN_FAILE;
    }

    evhttp_set_timeout(m_pHttp, Http_TimeOut);
    evhttp_set_gencb(m_pHttp, workThreadHttpCB, m_objSessionManager.getInterface());

    return Q_RTN_OK;
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
        -1, EV_PERSIST, workThreadTimerCB, &m_objSessionManager);
    if (NULL == m_pEvent)
    {
        Q_Printf("%s", "event_new error");

        return Q_RTN_FAILE;
    }

    (void)event_priority_set(m_pEvent, Priority_Hight);
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

void CWorkThreadEvent::onWebSockRead(struct SockPairEventParam *pParam)
{
    struct bufferevent *pBev = NULL;
    Q_SOCK iFD = Q_INVALID_SOCK;
    CSession *pSession = NULL;
    CSessionManager *pSessionManager = (CSessionManager *)(pParam->pUserDate);

    while(Q_GetEventValue<Q_SOCK>(pParam->pEventBuf, iFD))
    {
        if (Q_INVALID_SOCK == iFD)
        {
            Q_Printf("%s", "invalid socket");

            continue;
        }

        (void)evutil_make_socket_nonblocking(iFD);
        pBev = bufferevent_socket_new(pParam->pMainBase, iFD, 
            BEV_OPT_CLOSE_ON_FREE);
        if (NULL == pBev)
        {
            evutil_closesocket(iFD);
            Q_Printf("%s", "bufferevent_socket_new error.");

            continue;
        }

        if (Q_RTN_OK != pSessionManager->addSession(pBev))
        {
            evutil_closesocket(iFD);
            bufferevent_free(pBev);
            Q_Printf("%s", "add session error.");

            continue;
        }

        bufferevent_setcb(pBev, workThreadReadCB, NULL, workThreadEventCB, 
            pSessionManager);
        if (Q_RTN_OK != bufferevent_enable(pBev, EV_READ | EV_WRITE))
        {
            evutil_closesocket(iFD);
            pSessionManager->dellSession(pBev);
            bufferevent_free(pBev);

            Q_Printf("%s", "bufferevent_enable error.");

            continue;
        }

        pSession = pSessionManager->getSession(pBev);
        pSession->setType(SType_WebSock);
        pSession->setStatus(SessionStatus_Connect);
    }
}

void CWorkThreadEvent::onTcpRead(struct SockPairEventParam *pParam)
{
    struct bufferevent *pBev = NULL;
    Q_SOCK iFD = Q_INVALID_SOCK;
    CSession *pSession = NULL;
    CSessionManager *pSessionManager = (CSessionManager *)(pParam->pUserDate);

    while(Q_GetEventValue<Q_SOCK>(pParam->pEventBuf, iFD))
    {
        if (Q_INVALID_SOCK == iFD)
        {
            Q_Printf("%s", "invalid socket");

            continue;
        }

        (void)evutil_make_socket_nonblocking(iFD);
        pBev = bufferevent_socket_new(pParam->pMainBase, iFD, 
            BEV_OPT_CLOSE_ON_FREE);
        if (NULL == pBev)
        {
            evutil_closesocket(iFD);
            Q_Printf("%s", "bufferevent_socket_new error.");

            continue;
        }
       
        if (Q_RTN_OK != pSessionManager->addSession(pBev))
        {
            evutil_closesocket(iFD);
            bufferevent_free(pBev);
            Q_Printf("%s", "add session error.");

            continue;
        }

        bufferevent_setcb(pBev, workThreadReadCB, NULL, workThreadEventCB, 
            pSessionManager);
        if (Q_RTN_OK != bufferevent_enable(pBev, EV_READ | EV_WRITE))
        {
            evutil_closesocket(iFD);
            pSessionManager->dellSession(pBev);
            bufferevent_free(pBev);

            Q_Printf("%s", "bufferevent_enable error.");

            continue;
        }

        pSession = pSessionManager->getSession(pBev);
        pSession->setType(SType_TcpClient);
        pSession->setStatus(SessionStatus_Linked);
    }
}

void CWorkThreadEvent::onOrderRead(struct SockPairEventParam *pParam)
{
    OrderMsg stOrderMsg;
    CSessionManager *pSessionManager = (CSessionManager *)(pParam->pUserDate);

    while(Q_GetEventValue<OrderMsg>(pParam->pEventBuf, stOrderMsg))
    {
        switch(stOrderMsg.emType)
        {
        case OrderType_AddServerLinker:
            addServerLinker(pParam->pMainBase, pSessionManager, stOrderMsg);
            break;

        default:
            break;
        }
    }
}

void CWorkThreadEvent::addServerLinker(struct event_base *pMainBase, 
    CSessionManager *pSessionManager, OrderMsg &stOrderMsg)
{
    struct bufferevent *pBev = NULL;
    CSession *pSession = NULL;
    CServerLinker *pServerLinker = (CServerLinker *)stOrderMsg.pHandle;

    if (NULL == pServerLinker)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return;
    }

    if (Q_INVALID_SOCK == pServerLinker->getSock())
    {
        Q_Printf("%s", "invalid socket");

        return;
    }

    (void)evutil_make_socket_nonblocking(pServerLinker->getSock());
    pBev = bufferevent_socket_new(pMainBase, pServerLinker->getSock(), 
        BEV_OPT_CLOSE_ON_FREE);
    if (NULL == pBev)
    {
        Q_Printf("%s", "bufferevent_socket_new error.");

        return;
    }

    if (Q_RTN_OK != pSessionManager->addSession(pBev))
    {
        bufferevent_free(pBev);
        Q_Printf("%s", "add session error.");

        return;
    }

    bufferevent_setcb(pBev, workThreadReadCB, NULL, workThreadEventCB, 
        pSessionManager);
    if (Q_RTN_OK != bufferevent_enable(pBev, EV_READ | EV_WRITE))
    {
        pSessionManager->dellSession(pBev);
        bufferevent_free(pBev);
        pBev = NULL;

        Q_Printf("%s", "bufferevent_enable error.");

        return;
    }
    
    pSessionManager->addServerLinker(pServerLinker->getLinkerName(), pBev);
    pSession = pSessionManager->getSession(pBev);    
    pSession->setHandle(pServerLinker);
    pSession->setType(SType_SVLinker);
    pSession->setStatus(SessionStatus_Linked);
    pServerLinker->setLinked(true); 

    pSessionManager->getInterface()->onLinkedServer(pSession);
}

void CWorkThreadEvent::onStop(struct SockPairEventParam *pParam)
{
    CSessionManager *pSessionManager = (CSessionManager *)(pParam->pUserDate);

    pSessionManager->getInterface()->onSerciveShutDown();
}

char *CWorkThreadEvent::getTcpDataPack(CSession *pSession, Q_PackHeadType &usSize)
{
    char *pTmp = NULL;
    usSize = Q_INIT_NUMBER;
    size_t iHeadSize = sizeof(Q_PackHeadType);

    size_t iTotalLens = pSession->getBuffer()->getTotalLens();
    if (iHeadSize > iTotalLens)
    {
        return NULL;
    }

    pTmp = pSession->getBuffer()->readBuffer(iHeadSize);
    if (NULL == pTmp)
    {
        return NULL;
    }

    Q_PackHeadType usPackLens = *((Q_PackHeadType*)pTmp);
    usPackLens = Q_NTOH(usPackLens);
    if (0 == usPackLens)
    {
        pSession->getBuffer()->delBuffer(iHeadSize);

        return NULL;
    }

    if (usPackLens > (iTotalLens - iHeadSize))
    {
        return NULL;
    }
    
    pTmp = pSession->getBuffer()->readBuffer(usPackLens + iHeadSize);
    if (NULL == pTmp)
    {
        return NULL;
    }
    
    usSize = usPackLens;

    return pTmp + iHeadSize;
}

void CWorkThreadEvent::workThreadReadCB(struct bufferevent *bev, void *arg)
{ 
    CSessionManager *pSessionManager = (CSessionManager *)arg;
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
    case SType_SVLinker:
    case SType_TcpClient:
        {
            char *pBuffer = NULL; 
            Q_PackHeadType packLens = Q_INIT_NUMBER;

            while(NULL != (pBuffer = getTcpDataPack(pSession, packLens)))
            {
                pSessionManager->getInterface()->onSocketRead(pBuffer, packLens);
                if (SessionStatus_Closed != pSession->getStatus())
                {
                    (void)pSession->getBuffer()->delBuffer(packLens + sizeof(packLens));
                }
                else
                {
                    break;
                }
            }
        }        
        break;

    case SType_WebSock:
        {
            bool bClose = false;
            CWebSock *pWebSock = pSessionManager->getWebSock();
            if (SessionStatus_Connect == pSession->getStatus())
            { 
                //握手
                if (pWebSock->shakeHands(bClose))
                {
                    pSession->setStatus(SessionStatus_Linked);
                }

                if (bClose)
                {
                    pSessionManager->closeCurLink();
                }
            }
            else
            {
                //处理数据
                pWebSock->dispData(bClose);
                if (bClose)
                {
                    pSessionManager->closeCurLink();
                }
            }
        }

    default:
        break;
    }    
    pSessionManager->setCurSession(NULL);
}

void CWorkThreadEvent::workThreadEventCB(struct bufferevent *bev, short event, void *arg)
{
    CSessionManager *pSessionManager = (CSessionManager *)arg;
    CSession *pSession = pSessionManager->getSession(bev);
    if (NULL == pSession)
    {
        bufferevent_free(bev);

        return;
    }

    pSessionManager->setCurSession(pSession);
    pSessionManager->getInterface()->onSocketClose();
    pSessionManager->setCurSession(NULL);

    if (SType_SVLinker == pSession->getType())
    {
        CServerLinker *pServerLinker = (CServerLinker *)pSession->getHandle();
        if (NULL != pServerLinker)
        {
            pSessionManager->delServerLinker(pServerLinker->getLinkerName());
            pServerLinker->setLinked(false);
        }
    }
    if (SType_WebSock == pSession->getType())
    {
        pSessionManager->getWebSock()->delContinuation(pSession->getBuffer()->getFD());
    }

    pSessionManager->dellSession(bev);
    bufferevent_free(bev);
}

void CWorkThreadEvent::workThreadTimerCB(evutil_socket_t, short event, void *arg)
{
    CSessionManager *pSessionManager = (CSessionManager *)arg;

    pSessionManager->addCount();

    pSessionManager->getInterface()->onTimerEvent();
}

void CWorkThreadEvent::workThreadHttpCB(struct evhttp_request *req, void *arg)
{
    CEventInterface *pInterface = (CEventInterface *)arg;
    CHttpBuffer objHttpEvent(req);
    if (objHttpEvent.isOK())
    {
        pInterface->onHttpRead(&objHttpEvent);
    }
}
