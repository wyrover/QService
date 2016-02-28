
#include "WorkThreadEvent.h"
#include "SessionManager.h"
#include "LinkOther.h"
#include "CommEncrypt.h"

#define Http_TimeOut  10
SINGLETON_INIT(CWorkThreadEvent)
CWorkThreadEvent objWorkThreadEvent;

CWorkThreadEvent::CWorkThreadEvent(void) : m_objWebSockParser(), m_objTcpParser(), m_objHttpParser()
{

}

CWorkThreadEvent::~CWorkThreadEvent(void)
{
    try
    {
        std::vector<struct evhttp *>::iterator itHttp;
        for (itHttp = m_vcEvHttp.begin(); m_vcEvHttp.end() != itHttp; itHttp++)
        {
            evhttp_free(*itHttp);
        }
        m_vcEvHttp.clear();
    }
    catch(...)
    {

    }
}

CWebSockParser *CWorkThreadEvent::getWebSockParser(void)
{
    return &m_objWebSockParser;
}

CTcpParser *CWorkThreadEvent::getTcpParser(void)
{
    return &m_objTcpParser;
}

CHttpParser *CWorkThreadEvent::getHttpParser(void)
{
    return &m_objHttpParser;
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

void CWorkThreadEvent::onMainRead(CEventBuffer *)
{
    struct bufferevent *pBev = NULL;
    CSession *pSession = NULL;
    CSessionManager *pSessionMgr = CSessionManager::getSingletonPtr();

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

        if (Q_RTN_OK != pSessionMgr->addSession(pBev))
        {
            evutil_closesocket(m_stWorkSock.iSock);
            bufferevent_free(pBev);
            Q_Printf("%s", "add session error.");

            continue;
        }

        bufferevent_setcb(pBev, workThreadReadCB, NULL, workThreadEventCB, NULL);
        if (Q_RTN_OK != bufferevent_enable(pBev, EV_READ | EV_WRITE))
        {
            evutil_closesocket(m_stWorkSock.iSock);
            pSessionMgr->dellSession(pBev);
            bufferevent_free(pBev);

            Q_Printf("%s", "bufferevent_enable error.");

            continue;
        }

        pSession = pSessionMgr->getSession(pBev);
        pSession->setType(m_stWorkSock.emType);
        pSession->setStatus(SESSSTATUS_LINKED);
        if (STYPE_WEBSOCK == m_stWorkSock.emType)
        {
            pSession->setStatus(SESSSTATUS_CONNECT);
        }

        if (STYPE_DEBUG != pSession->getType())
        {
            pSessionMgr->getInterface()->onConnected(pSession);
        }       
    }
}

void CWorkThreadEvent::onAssistRead(CEventBuffer *)
{
    struct bufferevent *pBev = NULL;
    CSession *pSession = NULL;
    CSessionManager *pSessionMgr = CSessionManager::getSingletonPtr();
    CLinkOther *pLinkOther = CLinkOther::getSingletonPtr();

    while(Q_GetEventValue<TriggerSock>(getAssistBuffer(), m_stWorkSock))
    {
        (void)evutil_make_socket_nonblocking(m_stWorkSock.iSock);
        pBev = bufferevent_socket_new(getBase(), m_stWorkSock.iSock, 
            BEV_OPT_CLOSE_ON_FREE);
        if (NULL == pBev)
        {
            pLinkOther->setSockStatus(m_stWorkSock.iSock, SESSSTATUS_CLOSED);            
            Q_Printf("%s", "bufferevent_socket_new error.");

            continue;
        }

        if (Q_RTN_OK != pSessionMgr->addSession(pBev))
        {
            pLinkOther->setSockStatus(m_stWorkSock.iSock, SESSSTATUS_CLOSED);
            bufferevent_free(pBev);
            Q_Printf("%s", "add session error.");

            continue;
        }

        bufferevent_setcb(pBev, workThreadReadCB, NULL, workThreadEventCB, NULL);
        if (Q_RTN_OK != bufferevent_enable(pBev, EV_READ | EV_WRITE))
        {
            pLinkOther->setSockStatus(m_stWorkSock.iSock, SESSSTATUS_CLOSED);
            pSessionMgr->dellSession(pBev);
            bufferevent_free(pBev);

            Q_Printf("%s", "bufferevent_enable error.");

            continue;
        }

        pSession = pSessionMgr->getSession(pBev);
        pSession->setType(m_stWorkSock.emType);
        pSession->setStatus(SESSSTATUS_LINKED);
        pLinkOther->setSockStatus(m_stWorkSock.iSock, SESSSTATUS_LINKED);

        pSessionMgr->getInterface()->onLinkedOther(pSession);
    }
}

void CWorkThreadEvent::onTimerRead(CEventBuffer *)
{
    char cVal = 0;
    CSessionManager *pSessionMgr = CSessionManager::getSingletonPtr();

    while(Q_GetEventValue<char>(getTimerBuffer(), cVal))
    {
        if (1 == cVal)
        {
            pSessionMgr->addCount();
            pSessionMgr->getInterface()->onTimerEvent();
        }
    }
}

bool CWorkThreadEvent::onStartUp(void)
{
    return CSessionManager::getSingleton().getInterface()->onSerciveStartUp();
}

void CWorkThreadEvent::onStop(void)
{
    CSessionManager::getSingleton().getInterface()->onSerciveShutDown();
}

void CWorkThreadEvent::dispTcp(class CSession *pSession)
{
    size_t iMsgLens = Q_INIT_NUMBER;
    CTcpParser *pParser = CWorkThreadEvent::getSingletonPtr()->getTcpParser();
    bool bSVLink = (STYPE_TCPCLIENT == pSession->getType()) ? true : false;

    while(true)
    {
        const char *pszBuf = pParser->parsePack(pSession->getBuffer());
        if (NULL == pszBuf)
        {
            return;
        }

        //解密处理
        const char *pszMsg = CCommEncrypt::getSingletonPtr()->Decode(pszBuf, pParser->getBufLens(), iMsgLens, bSVLink);
        if (NULL == pszMsg
            && NULL != pszBuf)
        {
            return;
        }

        if (STYPE_DEBUG == pSession->getType())
        {
            CSessionManager::getSingletonPtr()->getInterface()->onDebug(pszMsg, iMsgLens);
        }
        else
        {
            CSessionManager::getSingletonPtr()->getInterface()->onSockRead(pszMsg, iMsgLens);
        }

        if (SESSSTATUS_CLOSED != pSession->getStatus())
        {
            (void)pSession->getBuffer()->delBuffer(pParser->getParsedLens());
        }
        else
        {
            return;
        }
    }
}

void CWorkThreadEvent::dispWebSock(CSession *pSession)
{
    CWebSockParser *pParser = CWorkThreadEvent::getSingletonPtr()->getWebSockParser();
    CSessionManager *pSessionMgr = CSessionManager::getSingletonPtr();
    CWorkThreadEvent *pWorkThread = CWorkThreadEvent::getSingletonPtr();
    size_t iMsgLens = Q_INIT_NUMBER;

    //握手处理
    if (SESSSTATUS_CONNECT == pSession->getStatus())
    {
        const std::string *pStrShakeHandsRtn = pParser->shakeHands(pSession->getBuffer());
        if (pParser->getClose())
        {
            pSessionMgr->closeLinkByID(pSession->getSessionID());

            return;
        }
        if (NULL == pStrShakeHandsRtn)
        {
            return;
        }

        (void)pSession->getBuffer()->writeBuffer(pStrShakeHandsRtn->c_str(), pStrShakeHandsRtn->size());
        pSession->setStatus(SESSSTATUS_LINKED);

        (void)pSession->getBuffer()->delBuffer(pParser->getParsedLens());

        return;
    }
    
    //正常数据处理
    bool bOk = false;
    bool bControl = false;
    while(true)
    {
        //解包
        bOk = pParser->parsePack(pSession->getBuffer());
        if (pParser->getClose())
        {
            pWorkThread->delContinuation(pSession->getBuffer()->getFD());
            pSessionMgr->closeLinkByID(pSession->getSessionID());

            return;
        }
        if (!bOk)
        {
            return;
        }

        const WebSockFram * pHead = pParser->getHead();

        //控制帧
        bControl = false;
        switch(pHead->emOpCode)
        {
        case WSOCK_CLOSE:
            {
                pWorkThread->delContinuation(pSession->getBuffer()->getFD());
                pSessionMgr->closeLinkByID(pSession->getSessionID());

                return;
            }
        case WSOCK_PING:
            {
                bControl = true;
                size_t iOutLens = Q_INIT_NUMBER;
                const char *pPingHead = pParser->createHead(true, WSOCK_PONG, 0, iOutLens);

                (void)pSession->getBuffer()->writeBuffer(pPingHead, iOutLens);
                pSession->setPing(pSessionMgr->getCount());
            }
            break;
        case WSOCK_PONG:
            {
                bControl = true;
                pSession->setPing(pSessionMgr->getCount());
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
        if ((WSOCK_CONTINUATION != pHead->emOpCode)
            && (1 == pHead->cFin))
        {
            //解密处理
            const char *pszMsg = CCommEncrypt::getSingletonPtr()->Decode(pParser->getMsg(), pHead->uiDataLens, iMsgLens);
            if (NULL == pszMsg
                && NULL != pParser->getMsg())
            {
                return;
            }

            pSessionMgr->getInterface()->onSockRead(pszMsg, iMsgLens);
            if (SESSSTATUS_CLOSED == pSession->getStatus())
            {
                pWorkThread->delContinuation(sock);
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
            && (WSOCK_CONTINUATION == pHead->emOpCode))
        {
            std::string *pStrBuf = pWorkThread->getContinuation(sock);
            if (NULL != pStrBuf)
            {
                const char *pszMsg = CCommEncrypt::getSingletonPtr()->Decode(pParser->getMsg(), pHead->uiDataLens, iMsgLens);
                if (NULL == pszMsg
                    && NULL != pParser->getMsg())
                {
                    return;
                }

                pStrBuf->append(pszMsg, iMsgLens);
                pSessionMgr->getInterface()->onSockRead(pStrBuf->c_str(), pStrBuf->size());
                if (SESSSTATUS_CLOSED == pSession->getStatus())
                {
                    pWorkThread->delContinuation(sock);
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
            //解密处理
            const char *pszMsg = CCommEncrypt::getSingletonPtr()->Decode(pParser->getMsg(), pHead->uiDataLens, iMsgLens);
            if (NULL == pszMsg
                && NULL != pParser->getMsg())
            {
                return;
            }

            pWorkThread->addContinuation(sock, pszMsg, iMsgLens);
            (void)pSession->getBuffer()->delBuffer(pParser->getParsedLens());
        }
    }
}

void CWorkThreadEvent::workThreadReadCB(struct bufferevent *bev, void *arg)
{ 
    CSessionManager *pSessionManager = CSessionManager::getSingletonPtr();
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
    case STYPE_DEBUG:
    case STYPE_TCPCLIENT:
    case STYPE_TCP:
        {
            dispTcp(pSession);
        }
        break;

    case STYPE_WEBSOCK:
        {
            dispWebSock(pSession);
        }
        break;

    default:
        break;
    }    
    pSessionManager->setCurSession(NULL);
}

void CWorkThreadEvent::workThreadEventCB(struct bufferevent *bev, short, void *arg)
{
    CWorkThreadEvent *pWorkThreadEvent = CWorkThreadEvent::getSingletonPtr();
    CSessionManager *pSessionManager = CSessionManager::getSingletonPtr();
    CSession *pSession = pSessionManager->getSession(bev);
    if (NULL == pSession)
    {
        bufferevent_free(bev);

        return;
    }

    pSessionManager->getInterface()->onSockClose(pSession);

    switch(pSession->getType())
    {
    case STYPE_WEBSOCK:
        {
            pWorkThreadEvent->delContinuation(pSession->getBuffer()->getFD());
        }
        break;

    case STYPE_TCPCLIENT:
        {
            CLinkOther::getSingletonPtr()->setSockStatus(pSession->getBuffer()->getFD(), 
                SESSSTATUS_CLOSED);
        }
        break;

    default:
        break;
    }
    
    pSessionManager->dellSession(bev);
    bufferevent_free(bev);
}

void CWorkThreadEvent::workThreadHttpCB(struct evhttp_request *req, void *arg)
{
    CWorkThreadEvent *pWorkThreadEvent = CWorkThreadEvent::getSingletonPtr();
    CSessionManager *pSessionManager = CSessionManager::getSingletonPtr();
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
        (void)m_mapWebSockPack.insert(std::make_pair(sock, strBuf));

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
