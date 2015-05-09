
#ifndef Q_SERVER_H_
#define Q_SERVER_H_

#include "WorkThreadEvent.h"

class CServer
{
public:
    CServer(void);
    ~CServer(void);

    /*等待启动完成*/
    bool waitForStarted(void); 
    /*获取工作线程事件对象*/
    CWorkThreadEvent *getServerThreadEvent(void);
    /*状态设置*/
    void setRunStatus(RunStatus emStatus);
    RunStatus *getRunStatus(void);
    /*获取类型*/
    SessionType getSockType(evutil_socket_t uiSock);
    /*获取线程数*/
    unsigned short *getThreadNum(void)
    {
        return &m_usThreadNum;
    };
    
    struct event_base *getBase(void)
    {
        return m_pMainBase;
    };

    /*初始化*/
    int Init(const unsigned short &usThreadNum, const unsigned int &uiTime,
        std::vector<CEventInterface * > &vcInterface,
        std::map<unsigned short, std::string> &mapTcp, 
        std::map<unsigned short, std::string> &mapWebSock, 
        std::map<unsigned short, std::string> &mapHttp);
    /*启动服务*/
    int Start(void);
    /*停止服务*/
    void Stop(void);

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
    int initWorkThread(std::vector<CEventInterface * > &vcInterface);
    int Loop(void);
    void exitWorkThread(void);
    void freeMainEvent(void);
    bool getError(void);
    bool getIsRun(void);
    int initExitMonitor(unsigned int uiMS);
    Q_SOCK initHttpSock(const char *pszIp, const unsigned short &usPort) const;

private:
    RunStatus m_emRunStatus;
    unsigned short m_usThreadNum;
    unsigned int m_uiTimer;
    struct event_base *m_pMainBase;
    class CThreadPool *m_pPool;
    CWorkThreadEvent *m_pServerThreadEvent;
    struct event *m_pExitEvent;
    CQMutex m_objMutex_Exit;
    CCond m_objCond_Exit;
    TriggerSock m_stWorkSock;
    std::vector<Q_SOCK> m_vcHttpSock;
    std::vector<struct evconnlistener * > m_vcAllListener;
    std::map<evutil_socket_t, SessionType> m_mapType;
};

#endif//Q_SERVER_H_
