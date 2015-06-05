
#include "SessionManager.h"
#include "LinkOther.h"
#include "LuaFunc.h"
#include "WorkThreadEvent.h"

#define Q_INITSESSIONSIZE 1024
SINGLETON_INIT(CSessionManager)
CSessionManager objSessionManager;

CSessionManager::CSessionManager(void) : m_uiTimer(Q_INIT_NUMBER),
    m_uiCount(Q_INIT_NUMBER), m_pLua(NULL), m_pCurrent(NULL), m_pInterface(NULL),
    m_objSessionPool(Q_INITSESSIONSIZE, POOLNAM_SESSION), m_objWebSockParser(), 
    m_objTcpParser(), m_objNetBinary()
{
   
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
    m_objNetBinary.setLua(pLua);
    m_objSerializeBinary.setLua(pLua);
}

void CSessionManager::setCurSession(CSession *pSession)
{
    m_pCurrent = pSession;
}

void CSessionManager::dellSession(struct bufferevent *bev)
{
    std::tr1::unordered_map<Q_SOCK, CSession *>::iterator itSession;    
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
    m_objSessionPool.Free(pSession);
    m_umapSession.erase(itSession);

    return;
}

int CSessionManager::addSession(struct bufferevent *bev)
{
    int iRtn = Q_RTN_OK;
    CSession *pSession = NULL;

    pSession = m_objSessionPool.Malloc();    
    iRtn = pSession->getBuffer()->setBuffer(bev);
    if (Q_RTN_OK != iRtn)
    {
        pSession->Clear();
        m_objSessionPool.Free(pSession);

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

    std::tr1::unordered_map<Q_SOCK, CSession *>::iterator itSession;
    itSession = m_umapSession.find(fd);
    if (m_umapSession.end() == itSession)
    {
        return NULL;
    }

    return itSession->second;
}

void CSessionManager::confirmStop(void)
{
    CWorkThreadEvent::getSingletonPtr()->setRunStatus(RUNSTATUS_STOPPED);
}

bool CSessionManager::addLinkOther(const char *pszIp, const unsigned short usPort, 
    const char *pszName, const unsigned short usType)
{
    return CLinkOther::getSingleton().addHost(pszIp, usPort, pszName, usType);
}

int CSessionManager::getLinkOtherID(const char *pszName)
{
    assert(NULL != pszName);

    return (int)CLinkOther::getSingleton().getSockByName(pszName);
}

luabridge::LuaRef CSessionManager::getLinkOtherByType(const unsigned short usType)
{
    std::vector<int> vcSock = CLinkOther::getSingleton().getSockByType(usType);

    return Q_VectorToLua(m_pLua, &vcSock);
}

unsigned int CSessionManager::getSessionSize(void) const
{
    return (unsigned int)m_umapSession.size();
}

void CSessionManager::freeAllSession(void)
{
    std::tr1::unordered_map<Q_SOCK, CSession *>::iterator itSession;
    for (itSession = m_umapSession.begin(); m_umapSession.end() != itSession; itSession++)
    {
        bufferevent_free(itSession->second->getBuffer()->getBuffer());
        m_objSessionPool.Free(itSession->second);
    }
    m_umapSession.clear();
}

void CSessionManager::closeLinkByID(const int iID)
{
    if (Q_INVALID_SOCK == iID)
    {
        return;
    }

    std::tr1::unordered_map<Q_SOCK, CSession *>::iterator itSession;
    itSession = m_umapSession.find(iID);
    if (m_umapSession.end() == itSession)
    {
        return;
    }

    if (NULL != m_pInterface)
    {
        m_pInterface->onSockClose(itSession->second);
    }    

    switch(itSession->second->getType())
    {
    case STYPE_WEBSOCK:
        {
            //外面已经清理 CWorkThreadEvent::dispWebSock
        }
        break;

    case STYPE_TCPCLIENT:
        {
            CLinkOther::getSingleton().setSockStatus(iID, SESSSTATUS_CLOSED);
        }
        break;

    default:
        break;
    }

    bufferevent_free(itSession->second->getBuffer()->getBuffer());
    itSession->second->Clear();

    m_objSessionPool.Free(itSession->second);
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
    if (Q_INVALID_SOCK == iID)
    {
        return NULL;
    }

    std::tr1::unordered_map<Q_SOCK, CSession *>::iterator itSession;

    itSession = m_umapSession.find(iID);
    if (m_umapSession.end() == itSession)
    {
        return NULL;
    }

    return itSession->second;
}

bool CSessionManager::sendWebSock(CSession *pCurrent, const char *pszData, const unsigned int &uiLens)
{
    size_t iOutLens = Q_INIT_NUMBER;
    const char *pszHead = m_objWebSockParser.createHead(true, WSOCK_TEXTFRAME, uiLens, iOutLens);

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

bool CSessionManager::sendTcp(CSession *pCurrent, const char *pszData, const unsigned int &uiLens)
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

bool CSessionManager::sendToCur(const char *pszData, const unsigned int uiLens)
{
    if (NULL == m_pCurrent)
    {
        Q_Printf("%s", "current session pointer is null.");
        return false;
    }

    bool bOK = false;
    switch(m_pCurrent->getType())
    {
    case STYPE_DEBUG:
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

bool CSessionManager::sendToByID(const int iID, const char *pszData, const unsigned int uiLens)
{
    if (Q_INVALID_SOCK == iID)
    {
        Q_Printf("%s", "invalid socket.");
        return false;
    }

    std::tr1::unordered_map<Q_SOCK, CSession *>::iterator itSession;
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
    case STYPE_DEBUG:
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
    return sendToCur(m_objNetBinary.getBufferMgr()->getBuffer(), (unsigned int)m_objNetBinary.getBufferMgr()->getLens());
}

bool CSessionManager::sendBToByID(const int iID)
{
    return sendToByID(iID, m_objNetBinary.getBufferMgr()->getBuffer(), (unsigned int)m_objNetBinary.getBufferMgr()->getLens());
}
