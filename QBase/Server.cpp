
#include "Server.h"
#include "Thread.h"

SINGLETON_INIT(CServer)
CServer objServer;

CServer::CServer(void)
{
    m_uiTime = Q_INIT_NUMBER;
    m_pMainBase = NULL;
    m_pExitEvent = NULL;
    m_pMainBase = m_objInitBase.getBase();
}

CServer::~CServer(void)
{
    try
    {
        freeMainEvent();
    }
    catch(...)
    {

    }
}

SessionType CServer::getSockType(evutil_socket_t uiSock)
{
    std::map<evutil_socket_t, SessionType>::iterator itType;
    itType = m_mapType.find(uiSock);
    if (m_mapType.end() == itType)
    {
        return STYPE_TCP;
    }

    return itType->second;
}

void CServer::listenerAcceptCB(struct evconnlistener *pListener, Q_SOCK sock, struct sockaddr *, 
    int, void *arg)
{ 
    CServer *pServer = CServer::getSingletonPtr();
    CWorkThreadEvent *pThreadEvent = CWorkThreadEvent::getSingletonPtr();

    if (RUNSTATUS_RUNING != pServer->getRunStatus())
    {
        evutil_closesocket(sock);
        return;
    }

    TriggerSock *pWorkSock = pServer->getTriggerSock();
    pWorkSock->emType = pServer->getSockType(evconnlistener_get_fd(pListener));
    pWorkSock->iSock = sock;
    
    if (Q_RTN_OK != pThreadEvent->sendMainMsg((const char*)pWorkSock, sizeof(TriggerSock)))
    {
        Q_Printf("add socket %d to thread error.", sock);

        evutil_closesocket(sock);
    }
}

void CServer::exitMonitorCB(evutil_socket_t, short, void *arg)
{
    CServer *pServer = CServer::getSingletonPtr();

    switch(pServer->getRunStatus())
    {
    case RUNSTATUS_STOPPING:
        {
            Q_Printf("ready stop listener service.");
            pServer->setRunStatus(RUNSTATUS_STOPPED);            
        }
        break;

    case RUNSTATUS_STOPPED:
        {
            event_base_loopbreak(pServer->getBase());
        }
        break;

    default:
        break;
    }
}

Q_SOCK CServer::initHttpSock(const char *pszIp, const unsigned short &usPort) const
{
    CNETAddr objAddr;
    Q_SOCK sock = Q_INVALID_SOCK;

    if (Q_RTN_OK != objAddr.setAddr(pszIp, usPort))
    {
        return Q_INVALID_SOCK;
    }

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
    
    if (bind(sock, objAddr.getAddr(), (int)objAddr.getAddrSize()) < 0)
    {
        Q_Printf("bind socket on port: %d ip: %s error.", usPort, pszIp);
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

struct evconnlistener * CServer::initListener(const char *pszHost, const unsigned short usPort)
{
    int iRtn = Q_RTN_OK;
    CNETAddr objAddr;

    iRtn = objAddr.setAddr(pszHost, usPort);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "set network addr error.");

        return NULL;
    }

    struct evconnlistener *pListener = evconnlistener_new_bind(m_pMainBase, listenerAcceptCB, NULL, 
        LEV_OPT_CLOSE_ON_FREE, -1, 
        objAddr.getAddr(), (int)objAddr.getAddrSize());
    if (NULL == pListener)
    {
        iRtn = EVUTIL_SOCKET_ERROR();
        Q_Printf("evconnlistener_new_bind on port %d error. error code %d, message %s", 
            usPort, iRtn, evutil_socket_error_to_string(iRtn));

        return NULL;
    }

    return pListener;
}

void CServer::exitWorkThread(void)
{
    CWorkThreadEvent::getSingletonPtr()->Stop();
    //放这里释放是因为CSessionManager为全局的，他对应的struct event_base *可能在他释放之前就已经释放，
    //造成执行bufferevent_free错误
    CSessionManager::getSingletonPtr()->freeAllSession();
}

void CServer::exitTimerTrigger(void)
{
    CTimerTrigger::getSingletonPtr()->Stop();
}

void CServer::freeMainEvent(void)
{
    std::vector<struct evconnlistener * >::iterator itListener;
    for (itListener = m_vcAllListener.begin(); m_vcAllListener.end() != itListener; itListener++)
    {
        evconnlistener_free(*itListener);
    }
    m_vcAllListener.clear();

    std::vector<Q_SOCK>::iterator itHttpSock;
    for (itHttpSock = m_vcHttpSock.begin(); m_vcHttpSock.end() != itHttpSock; itHttpSock++)
    {
        evutil_closesocket(*itHttpSock);
    }
    m_vcHttpSock.clear();

    if (NULL != m_pExitEvent)
    {
        event_free(m_pExitEvent);
        m_pExitEvent = NULL;
    }
}

int CServer::Loop(void)
{
    int iRtn = Q_RTN_OK;
    CThread objThread;

    setRunStatus(RUNSTATUS_STARTING);

    objThread.Execute(CWorkThreadEvent::getSingletonPtr(), false);
    if (!CWorkThreadEvent::getSingletonPtr()->waitForStarted())
    {
        setRunStatus(RUNSTATUS_ERROR);
        Q_Printf("%s", "wait work thread start error.");

        return Q_RTN_FAILE;
    }

    if (Q_INIT_NUMBER != m_uiTime)
    {
        objThread.Execute(CTimerTrigger::getSingletonPtr(), false);
        if (!CTimerTrigger::getSingletonPtr()->waitForStarted())
        {
            setRunStatus(RUNSTATUS_ERROR);
            Q_Printf("%s", "wait timer trigger start error.");

            return Q_RTN_FAILE;
        }
    }

    setRunStatus(RUNSTATUS_RUNING);
    Q_Printf("%s", "listener service running...");
    iRtn = event_base_dispatch(m_pMainBase);
    setRunStatus(RUNSTATUS_STOPPED);

    m_objMutex_Exit.Lock();
    m_objCond_Exit.Signal();
    m_objMutex_Exit.unLock();

    Q_Printf("%s", "listener service stopped.");

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
        -1, EV_PERSIST, exitMonitorCB, NULL);
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

int CServer::Init(const unsigned int &uiMS, class CEventInterface *pInterface,
    struct lua_State *pLua, 
    std::map<unsigned short, std::string> &mapDebug, 
    std::map<unsigned short, std::string> &mapTcp, 
    std::map<unsigned short, std::string> &mapWebSock, 
    std::map<unsigned short, std::string> &mapHttp,
    std::map<unsigned short, std::string> &mapSVLink)
{
    int iRtn = Q_RTN_OK;
    struct evconnlistener *pListener = NULL;
    std::map<unsigned short, std::string>::iterator itHost;
    
    m_uiTime = uiMS;
    CSessionManager::getSingletonPtr()->setTimer(m_uiTime);
    CSessionManager::getSingletonPtr()->setInterface(pInterface);
    CSessionManager::getSingletonPtr()->setLua(pLua);

    /*初始化定时器触发器*/
    if (Q_INIT_NUMBER != m_uiTime)
    {
        iRtn = CTimerTrigger::getSingletonPtr()->setTime(m_uiTime);
        if (Q_RTN_OK != iRtn)
        {
            return iRtn;
        }
    }

    /*初始化HTTP*/
    if (!mapHttp.empty())
    {
        Q_Printf("%s", "init http server...");
    }    
    for (itHost = mapHttp.begin(); mapHttp.end() != itHost; itHost++)
    {
        Q_SOCK httpSock = initHttpSock(itHost->second.c_str(), itHost->first);
        if (Q_INVALID_SOCK == httpSock)
        {
            return Q_RTN_FAILE;
        }

        m_vcHttpSock.push_back(httpSock);
    }

    CWorkThreadEvent::getSingletonPtr()->setHttpSock(m_vcHttpSock);

    /*初始化debug监听*/
    if (!mapDebug.empty())
    {
        Q_Printf("%s", "init debug server...");
    }
    for (itHost = mapDebug.begin(); mapDebug.end() != itHost; itHost++)
    {
        pListener = initListener(itHost->second.c_str(), itHost->first);
        if (NULL == pListener)
        {
            return Q_RTN_FAILE;
        }

        m_vcAllListener.push_back(pListener);
        (void)m_mapType.insert(std::make_pair(evconnlistener_get_fd(pListener), STYPE_DEBUG));
    } 

    /*初始化websock监听*/
    if (!mapWebSock.empty())
    {
        Q_Printf("%s", "init websock server...");
    }
    for (itHost = mapWebSock.begin(); mapWebSock.end() != itHost; itHost++)
    {
        pListener = initListener(itHost->second.c_str(), itHost->first);
        if (NULL == pListener)
        {
            return Q_RTN_FAILE;
        }

        m_vcAllListener.push_back(pListener);
        (void)m_mapType.insert(std::make_pair(evconnlistener_get_fd(pListener), STYPE_WEBSOCK));
    }    

    /*初始化tcp监听*/
    if (!mapTcp.empty())
    {
        Q_Printf("%s", "init tcp listener...");
    }
    for (itHost = mapTcp.begin(); mapTcp.end() != itHost; itHost++)
    {
        pListener = initListener(itHost->second.c_str(), itHost->first);
        if (NULL == pListener)
        {
            return Q_RTN_FAILE;
        }

        m_vcAllListener.push_back(pListener);
        (void)m_mapType.insert(std::make_pair(evconnlistener_get_fd(pListener), STYPE_TCP));
    }

    /*初始化服务器间链接监听*/
    if (!mapSVLink.empty())
    {
        Q_Printf("%s", "init server linker listener...");
    }
    for (itHost = mapSVLink.begin(); mapSVLink.end() != itHost; itHost++)
    {
        pListener = initListener(itHost->second.c_str(), itHost->first);
        if (NULL == pListener)
        {
            return Q_RTN_FAILE;
        }

        m_vcAllListener.push_back(pListener);
        (void)m_mapType.insert(std::make_pair(evconnlistener_get_fd(pListener), STYPE_TCPCLIENT));
    }

    /*初始化退出*/
    iRtn = initExitMonitor(100);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
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
        exitTimerTrigger();
        exitWorkThread();

        return;
    }
    
    exitTimerTrigger();
    exitWorkThread();

    m_objMutex_Exit.Lock();
    setRunStatus(RUNSTATUS_STOPPING);
    m_objCond_Exit.Wait(&m_objMutex_Exit);    
    m_objMutex_Exit.unLock();
}
