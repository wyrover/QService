
#include "LinkOther.h"
#include "WorkThreadEvent.h"
#include "Thread.h"

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

CLinkOther::CLinkOther(void) : m_emStatus(RunStatus_Unknown), m_pThreadEvent(NULL),
    m_objMutex()
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

RunStatus CLinkOther::getStatus(void) const
{
    return m_emStatus;
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

bool CLinkOther::addHost(const char *pszIp, unsigned short usPort, const char *pszName)
{
    if (checkHaveHost(pszName))
    {
        Q_Printf("link other name %s already exist.",  pszName);

        return false;
    }

    LinkInfo stLinkInfo;
    stLinkInfo.emStatus = SessionStatus_Closed;
    stLinkInfo.sock = Q_INVALID_SOCK;
    stLinkInfo.strIp = pszIp;
    stLinkInfo.strName = pszName;
    stLinkInfo.usPort = usPort;

    m_objMutex.Lock();
    m_vcLinkInfo.push_back(stLinkInfo);
    m_objMutex.unLock();

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

    if (Q_RTN_OK != connect(sock, objAddr.getAddr(), objAddr.getAddrSize()))
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

    m_objMutex.Lock();
    for (itInfo = m_vcLinkInfo.begin(); m_vcLinkInfo.end() != itInfo; itInfo++)
    {
        if (SessionStatus_Closed != itInfo->emStatus)
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

int CLinkOther::getSockByName(const char *pszName)
{
    if (NULL == pszName)
    {
        return Q_INVALID_SOCK;
    }

    int iSock = Q_INVALID_SOCK;
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
