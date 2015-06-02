
#ifndef Q_SERVER_H_
#define Q_SERVER_H_

#include "CTask.h"
#include "Singleton.h"
#include "WorkThreadEvent.h"
#include "SessionManager.h"
#include "TimerTrigger.h"
#include "RunStatus.h"

class CServer : public CTask,
    public CRunStatus,
    public CSingleton<CServer>
{
public:
    CServer(void);
    ~CServer(void);

    /*获取类型*/
    SessionType getSockType(evutil_socket_t uiSock);
   
    void Run(void)
    {
        Start();
    };

    /*初始化*/
    int Init(const unsigned int &uiMS, class CEventInterface *pInterface,
        struct lua_State *pLua,
        std::map<unsigned short, std::string> &mapDebug, 
        std::map<unsigned short, std::string> &mapTcp, 
        std::map<unsigned short, std::string> &mapWebSock, 
        std::map<unsigned short, std::string> &mapHttp);
    /*启动服务*/
    int Start(void);
    /*停止服务*/
    void Stop(void);

    struct event_base *getBase(void)
    {
        return m_pMainBase;
    };

    TriggerSock *getTriggerSock(void)
    {
        return &m_stWorkSock;
    };

public:
    static void listenerAcceptCB(struct evconnlistener *pListener, Q_SOCK sock, struct sockaddr *, 
        int, void *arg);
    static void exitMonitorCB(evutil_socket_t, short, void *arg);

private:
    struct evconnlistener * initListener(const char *pszHost, const unsigned short usPort);
    int Loop(void);
    void exitWorkThread(void);
    void exitTimerTrigger(void);
    void freeMainEvent(void);
    int initExitMonitor(unsigned int uiMS);
    Q_SOCK initHttpSock(const char *pszIp, const unsigned short &usPort) const;

private:
    unsigned int m_uiTime;
    struct event_base *m_pMainBase;
    struct event *m_pExitEvent;
    CQMutex m_objMutex_Exit;
    CCond m_objCond_Exit;
    TriggerSock m_stWorkSock;
    CInitBase m_objInitBase;
    std::vector<Q_SOCK> m_vcHttpSock;
    std::vector<struct evconnlistener * > m_vcAllListener;
    std::map<evutil_socket_t, SessionType> m_mapType;
};

#endif//Q_SERVER_H_
