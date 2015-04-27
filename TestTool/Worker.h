#ifndef Q_WORKER_H_
#define Q_WORKER_H_

#include "TransMsg.h"
#include "Reg2Lua.h"

class CWorker : public CSockPairEvent
{
public:
    CWorker(void);
    ~CWorker(void);

    bool initLua(void);
    void setTimer(unsigned int uiMS);

    struct lua_State * getLua(void)
    {
        return m_pLua;
    };
    CTcpParser *getTcpParser(void)
    {
        return &m_objTcpParser;
    };
    CSessionManager *getSessionManager(void)
    {
        return &m_objSessionManager;
    };

    //接口实现
public:
    void onMainRead(CEventBuffer *pBuffer);//input
    void onAssistRead(CEventBuffer *pBuffer);
    bool onStartUp(void);
    void onStop(void);

public:
    static void workThreadTimerCB(evutil_socket_t, short event, void *arg);   
    static void mainReadCB(struct bufferevent *bev, void *arg);
    static void mainEventCB(struct bufferevent *bev, short event, void *arg);
    static void assistReadCB(struct bufferevent *bev, void *arg);
    static void assistEventCB(struct bufferevent *bev, short event, void *arg);

private:
    void regMFCFunc(void);

private:
    struct event *m_pEvent;
    struct lua_State *m_pLua;
    CTcpParser m_objTcpParser;
    CReg2Lua m_objRegLua;
    CSessionManager m_objSessionManager;
};

#endif//Q_WORKER_H_
