
#include "LinkOther.h"
#include "WorkThreadEvent.h"
#include "Thread.h"

SINGLETON_INIT(CLinkOther)
CLinkOther objLinkOther;

class CLinkOtherTask : public CTask
{
public:
    CLinkOtherTask(void) : m_pLinker(NULL)
    {

    };
    ~CLinkOtherTask(void)
    {
        m_pLinker = NULL;
    };
    void setHandle(CLinkOther *pLinker)
    {
        m_pLinker = pLinker;
    };
    void Run(void)
    {
        Q_Printf("%s", "link other monitor service running...");
        m_pLinker->setRunStatus(RUNSTATUS_RUNING);
        unsigned long long ullCount = Q_INIT_NUMBER;

        while(RUNSTATUS_RUNING == m_pLinker->getRunStatus())
        {
            if (0 == (ullCount % 10))
            {
                m_pLinker->Link();
            }            

            Q_Sleep(100);
            ullCount++;
        }

        m_pLinker->setRunStatus(RUNSTATUS_STOPPED);

        Q_Printf("%s", "link other monitor service stopped.");
    };
private:
    CLinkOther *m_pLinker;
};

CLinkOther::CLinkOther(void) : m_objMutex()
{

}

CLinkOther::~CLinkOther(void)
{
    try
    {
        Stop();
    }
    catch(...)
    {

    }
}

bool CLinkOther::checkHaveHost(const char *pszName)
{
    bool bHave = false;
    std::string strHost = pszName;
    std::vector<LinkInfo>::iterator itInfo;

    m_objMutex.Lock();
    for (itInfo = m_vcLinkInfo.begin(); m_vcLinkInfo.end() != itInfo; itInfo++)
    {
        if (strHost == itInfo->strName)
        {
            bHave = true;
            break;
        }
    }
    m_objMutex.unLock();

    return bHave;
}

bool CLinkOther::addHost(const char *pszIp, const unsigned short usPort, 
    const char *pszName, const unsigned short usType)
{
    if (checkHaveHost(pszName))
    {
        Q_Printf("link other name %s already exist.",  pszName);

        return false;
    }

    LinkInfo stLinkInfo;
    stLinkInfo.emStatus = SESSSTATUS_CLOSED;
    stLinkInfo.sock = Q_INVALID_SOCK;
    stLinkInfo.strIp = pszIp;
    stLinkInfo.strName = pszName;
    stLinkInfo.usPort = usPort;
    stLinkInfo.usType = usType;

    m_objMutex.Lock();
    m_vcLinkInfo.push_back(stLinkInfo);
    m_objMutex.unLock();

    return true;
}

int CLinkOther::Start(void)
{
    CThread objThread;

    setRunStatus(RUNSTATUS_STARTING);
    CLinkOtherTask *pTask = new(std::nothrow)CLinkOtherTask(); 
    if (NULL == pTask)
    {
        setRunStatus(RUNSTATUS_ERROR);

        return Q_RTN_FAILE;
    }

    pTask->setHandle(this);

    objThread.Execute(pTask);
    if (!waitForStarted())
    {
        setRunStatus(RUNSTATUS_ERROR);

        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

void CLinkOther::Stop(void)
{
    if (RUNSTATUS_RUNING == getRunStatus())
    {
        setRunStatus(RUNSTATUS_STOPPING);
        while(true)
        {
            if (RUNSTATUS_STOPPED == getRunStatus())
            {
                break;
            }

            Q_Sleep(10);
        }
    }
}

Q_SOCK CLinkOther::initSock(const char *pszIp, const unsigned short &usPort) const
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

    if (Q_RTN_OK != connect(sock, objAddr.getAddr(), (int)objAddr.getAddrSize()))
    {
        evutil_closesocket(sock);
        return Q_INVALID_SOCK;
    }

    (void)setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, sizeof(iKeepAlive));

    return sock;
}

void CLinkOther::Link(void)
{
    Q_SOCK sok = Q_INVALID_SOCK;
    TriggerSock stTriggerSock;
    std::vector<LinkInfo>::iterator itInfo;

    if (!CWorkThreadEvent::getSingletonPtr()->getIsRun())
    {
        return;
    }

    m_objMutex.Lock();
    for (itInfo = m_vcLinkInfo.begin(); m_vcLinkInfo.end() != itInfo; itInfo++)
    {
        if (SESSSTATUS_CLOSED != itInfo->emStatus)
        {
            continue;
        }
        
        Q_Printf("try link %s, ip %s, port %d...", 
            itInfo->strName.c_str(), itInfo->strIp.c_str(), itInfo->usPort);
        sok = initSock(itInfo->strIp.c_str(), itInfo->usPort);
        if (Q_INVALID_SOCK == sok)
        {
            Q_Printf("%s", "link error...");
            continue;
        }

        stTriggerSock.emType = STYPE_TCPCLIENT;
        stTriggerSock.iSock = sok;
        if (Q_RTN_OK != CWorkThreadEvent::getSingletonPtr()->
                sendAssistMsg((const char *)&stTriggerSock, sizeof(stTriggerSock)))
        {
            evutil_closesocket(sok);
        }
        else
        {
            itInfo->emStatus = SESSSTATUS_CONNECT;
            itInfo->sock = sok;
        }
    }
    m_objMutex.unLock();
}

void CLinkOther::setSockStatus(Q_SOCK sock, SessionStatus emStatus)
{
    if (Q_INVALID_SOCK == sock)
    {
        return;
    }

    std::vector<LinkInfo>::iterator itInfo;

    m_objMutex.Lock();
    for (itInfo = m_vcLinkInfo.begin(); m_vcLinkInfo.end() != itInfo; itInfo++)
    {
        if (itInfo->sock == sock)
        {
            itInfo->emStatus = emStatus;
            break;
        }
    }
    m_objMutex.unLock();
}

Q_SOCK CLinkOther::getSockByName(const char *pszName)
{
    if (NULL == pszName)
    {
        return Q_INVALID_SOCK;
    }

    Q_SOCK iSock = Q_INVALID_SOCK;
    std::vector<LinkInfo>::iterator itInfo;

    m_objMutex.Lock();
    for (itInfo = m_vcLinkInfo.begin(); m_vcLinkInfo.end() != itInfo; itInfo++)
    {
        if (itInfo->strName == std::string(pszName))
        {
            iSock = itInfo->sock;
            break;
        }
    }
    m_objMutex.unLock();

    return iSock;
}

std::vector<int> CLinkOther::getSockByType(const unsigned short usType)
{
    std::vector<LinkInfo>::iterator itInfo;
    std::vector<int> vcSock;

    m_objMutex.Lock();
    for (itInfo = m_vcLinkInfo.begin(); m_vcLinkInfo.end() != itInfo; itInfo++)
    {
        if ((usType == itInfo->usType)
            && (Q_INVALID_SOCK != itInfo->sock))
        {
            vcSock.push_back((int)(itInfo->sock));
        }
    }
    m_objMutex.unLock();

    return vcSock;
}
