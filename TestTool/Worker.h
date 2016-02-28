#ifndef Q_WORKER_H_
#define Q_WORKER_H_

#include "TransMsg.h"
#include "Reg2Lua.h"
#include "ClientEncrypt.h"

class CWorker : public CSockPairEvent,
    public CTask,
    public CSingleton<CWorker>
{
public:
    CWorker(void);
    ~CWorker(void);

    bool initLua(void);
    void setTimer(unsigned int uiMS);

    void Run(void)
    {
        Start();
    };

    struct lua_State * getLua(void)
    {
        return m_pLua;
    };
    CTcpParser *getTcpParser(void)
    {
        return &m_objTcpParser;
    };

    //�ӿ�ʵ��
public:
    void onMainRead(CEventBuffer *pBuffer);//input
    void onAssistRead(CEventBuffer *);
    bool onStartUp(void);
    void onStop(void);

public:
    static void workThreadTimerCB(evutil_socket_t, short event, void *arg);   
    static void mainReadCB(struct bufferevent *bev, void *arg);
    static void mainEventCB(struct bufferevent *bev, short event, void *arg);

private:
    void regMFCFunc(void);

private:
    struct event *m_pEvent;
    struct lua_State *m_pLua;
    CTcpParser m_objTcpParser;
    CReg2Lua m_objRegLua;
};

extern int g_iChecked;
extern CQMutex g_objWorkerMutex;

enum WorkerCommandType
{
    WCOMM_CLOSEMAIN = 0,
};

struct WorkerCommand
{
    WorkerCommandType emType;
    Q_SOCK sock;
};

#endif//Q_WORKER_H_
