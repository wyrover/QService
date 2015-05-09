
#include "Server.h"
#include "ThreadPool.h"
#include "Exception.h"
#include "SysLog.h"

class CWorkerThreadTask : public CTask
{
public:
    CWorkerThreadTask(void) : m_pThreadEvent(NULL)
    {

    };
    ~CWorkerThreadTask(void)
    {
        m_pThreadEvent = NULL;
    };

    void setWorkerHandle(CWorkThreadEvent *pThreadEvent)
    {
        m_pThreadEvent = pThreadEvent;
    };

    void Run(void)
    {
        (void)m_pThreadEvent->Start();
    }

private:
    CWorkThreadEvent *m_pThreadEvent;
};

/*****************************************************************************/

CServer::CServer(void)
{
    m_uiTimer = Q_INIT_NUMBER;
    m_emRunStatus = RunStatus_Unknown;
    m_usThreadNum = 1;
    m_pMainBase = NULL;
    m_pPool = NULL;
    m_pServerThreadEvent = NULL;
    m_pExitEvent = NULL;
}

CServer::~CServer(void)
{
    try
    {
        Q_SafeDelete_Array(m_pServerThreadEvent);
        Q_SafeDelete(m_pPool);
        freeMainEvent();
    }
    catch(...)
    {

    }
}

bool CServer::getError(void)
{
    return ((RunStatus_Error == *getRunStatus()) ? true : false);
}

void CServer::setRunStatus(RunStatus emStatus)
{
    m_emRunStatus = emStatus;
}

RunStatus *CServer::getRunStatus(void)
{
    return &m_emRunStatus;
}

bool CServer::getIsRun(void)
{
    return ((RunStatus_Runing == *getRunStatus()) ? true : false);
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

CWorkThreadEvent *CServer::getServerThreadEvent(void)
{
    return m_pServerThreadEvent;
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
    unsigned short usIndex = Q_INIT_NUMBER;  
    CServer *pServer = (CServer *)arg;
    CWorkThreadEvent *pThreadEvent = pServer->getServerThreadEvent();

    if (RunStatus_Runing != *(pServer->getRunStatus()))
    {
        evutil_closesocket(sock);
        return;
    }

    //取得最小连接数的线程号
    if (*(pServer->getThreadNum()) > 1)
    {
        unsigned int uiCount = Q_INIT_NUMBER;
        unsigned int uiTmp = pThreadEvent[0].getSessionManager()->getSessionSize();
        for (unsigned short usI = Q_INIT_NUMBER; usI < *(pServer->getThreadNum()); usI++)
        {
            uiCount = pThreadEvent[usI].getSessionManager()->getSessionSize();
            if (uiTmp > uiCount)
            {
                uiTmp = uiCount;
                usIndex = usI;
            }
        }
    }

    TriggerSock *pWorkSock = pServer->getTriggerSock();
    pWorkSock->emType = pServer->getSockType(evconnlistener_get_fd(pListener));
    pWorkSock->iSock = sock;
    
    if (Q_RTN_OK != pThreadEvent[usIndex].sendMainMsg((const char*)pWorkSock, sizeof(TriggerSock)))
    {
        Q_Printf("add socket %d to thread %d error.", sock, usIndex);
        Q_SYSLOG(LOGLV_ERROR, "add socket %d to thread %d error.", sock, usIndex);

        evutil_closesocket(sock);
    }
}

void CServer::exitMonitorCB(evutil_socket_t, short, void *arg)
{
    CServer *pServer = (CServer *)arg;
    switch(*(pServer->getRunStatus()))
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
    
    if (bind(sock, objAddr.getAddr(), objAddr.getAddrSize()) < 0)
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

    struct evconnlistener *pListener = evconnlistener_new_bind(m_pMainBase, listenerAcceptCB, this, 
        LEV_OPT_CLOSE_ON_FREE, -1, 
        objAddr.getAddr(), objAddr.getAddrSize());
    if (NULL == pListener)
    {
        iRtn = EVUTIL_SOCKET_ERROR();
        Q_Printf("evconnlistener_new_bind on port %d error. error code %d, message %s", 
            usPort, iRtn, evutil_socket_error_to_string(iRtn));

        return NULL;
    }

    return pListener;
}

int CServer::initWorkThread(std::vector<CEventInterface * > &vcInterface)
{
    try
    {
        m_pServerThreadEvent = new(std::nothrow) CWorkThreadEvent[m_usThreadNum];
        if (NULL == m_pServerThreadEvent)
        {
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

            return Q_ERROR_ALLOCMEMORY;
        }
    }
    catch(CQException &e)
    {
        Q_Printf("get an exception. code %d, message %s", e.getErrorCode(), e.getErrorMsg());

        return e.getErrorCode();
    }

    for (unsigned short usI = 0; usI < m_usThreadNum; usI++)
    {
        m_pServerThreadEvent[usI].setInterface(vcInterface[usI]);         
        if (Q_RTN_OK != m_pServerThreadEvent[usI].setTimer(m_uiTimer))
        {
            return Q_RTN_FAILE;
        }

        if (Q_RTN_OK != m_pServerThreadEvent[usI].setHttpSock(m_vcHttpSock))
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

    for (int i = 0; i < m_usThreadNum; i++)
    {
        m_pServerThreadEvent[i].Stop();
    }
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

    for (unsigned short us = 0; us < m_usThreadNum; us++)
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

        m_pServerThreadEvent[us].getLinkOther()->setThreadEvent(&m_pServerThreadEvent[us]);
        if (!(m_pServerThreadEvent[us].getLinkOther()->Start()))
        {
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

int CServer::Init(const unsigned short &usThreadNum, const unsigned int &uiTime,
    std::vector<CEventInterface * > &vcInterface,
    std::map<unsigned short, std::string> &mapTcp, 
    std::map<unsigned short, std::string> &mapWebSock, 
    std::map<unsigned short, std::string> &mapHttp)
{
    int iRtn = Q_RTN_OK;
    struct evconnlistener *pListener = NULL;
    std::map<unsigned short, std::string>::iterator itHost;

    m_usThreadNum = ((0 == usThreadNum) ? 1 : usThreadNum);
    m_uiTimer = uiTime;

    setRunStatus(RunStatus_Starting);
    /*主线程循环*/
    m_pMainBase = event_base_new();
    if (NULL == m_pMainBase)
    {
        Q_Printf("%s", "event_base_new error.");
        setRunStatus(RunStatus_Error);

        return Q_RTN_FAILE;
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
            setRunStatus(RunStatus_Error);

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
            setRunStatus(RunStatus_Error);

            return Q_RTN_FAILE;
        }

        m_vcAllListener.push_back(pListener);
        (void)m_mapType.insert(std::make_pair(evconnlistener_get_fd(pListener), STYPE_TCP));
    }

    /*初始化退出*/
    iRtn = initExitMonitor(100);
    if (Q_RTN_OK != iRtn)
    {
        setRunStatus(RunStatus_Error);

        return iRtn;
    }

    /*初始化工作线程*/
    Q_Printf("%s", "init work thread...");
    iRtn = initWorkThread(vcInterface);
    if (Q_RTN_OK != iRtn)
    {
        setRunStatus(RunStatus_Error);

        return iRtn;
    }

    try
    {
        m_pPool = new(std::nothrow) CThreadPool(m_usThreadNum);
        if (NULL == m_pPool)
        {
            setRunStatus(RunStatus_Error);
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

            return Q_ERROR_ALLOCMEMORY;
        }
    }
    catch(CQException &e)
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
