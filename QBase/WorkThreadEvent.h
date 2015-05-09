
#ifndef Q_SERVERTHREAD_EVENT_H_
#define Q_SERVERTHREAD_EVENT_H_

#include "SockPairEvent.h"
#include "SessionManager.h"
#include "WebSockParser.h"
#include "TcpParser.h"
#include "HttpParser.h"

struct TriggerSock
{
    SessionType emType;
    Q_SOCK iSock;
    TriggerSock(void) : emType(STYPE_TCP), iSock(Q_INVALID_SOCK)
    {

    };
};

/*
* 工作线程事件
* 通信格式 unsigned short(消息长度) + 消息
*/
class CWorkThreadEvent : public CSockPairEvent
{
public:
    CWorkThreadEvent(void);
    ~CWorkThreadEvent(void);

    /*设置接口对象*/
    void setInterface(CEventInterface *pInterface);
    /*定时器时间(ms)*/
    int setTimer(unsigned int uiMS);
    int setHttpSock(std::vector<Q_SOCK> &vcHttpSock);

    /*获取该事件循环维护的CSessionManager*/
    CSessionManager *getSessionManager(void);
    /*获取WebSockParser*/
    CWebSockParser *getWebSockParser(void);
    /*获取CTcpParser*/
    CTcpParser *getTcpParser(void);
    /*获取链接到其他服务器对象*/
    CLinkOther *getLinkOther(void);
    CHttpParser *getHttpParser(void);

public:
    //接口实现
    void onMainRead(CEventBuffer *);
    void onAssistRead(CEventBuffer *);
    void onStop(void);
    bool onStartUp(void);

public:
    static void workThreadReadCB(struct bufferevent *bev, void *arg);
    static void workThreadEventCB(struct bufferevent *bev, short, void *arg);
    static void workThreadTimerCB(evutil_socket_t, short, void *arg);
    static void workThreadHttpCB(struct evhttp_request *req, void *arg);

    //不同类型协议读取
    static void dispTcp(CWorkThreadEvent *pWorkThreadEvent,
        CSessionManager *pSessionManager, CSession *pSession);
    static void dispWebSock(CWorkThreadEvent *pWorkThreadEvent,
        CSessionManager *pSessionManager, CSession *pSession);

public:
    //websocket 分片帧操作
    void addContinuation(const Q_SOCK &sock, const char *pszData, const size_t &iLens);
    std::string *getContinuation(const Q_SOCK &sock);
    void delContinuation(const Q_SOCK &sock);

private:
    struct event *m_pEvent;
    CSessionManager m_objSessionManager;
    TriggerSock m_stWorkSock;
    CWebSockParser m_objWebSockParser;
    CTcpParser m_objTcpParser;
    CHttpParser m_objHttpParser;
    std::vector<struct evhttp *> m_vcEvHttp;
    std::tr1::unordered_map<Q_SOCK, std::string> m_mapWebSockPack;
};

#endif//Q_SERVERTHREAD_EVENT_H_
