
#ifndef Q_LOG_H_
#define Q_LOG_H_

#include "SockPairEvent.h"
#include "TcpParser.h"
#include "Loger.h"

struct LogerInfo;
//日志
class CLog : public CSockPairEvent
{
public:
    CLog(void);
    ~CLog(void);

    //添加一日志记录器,返回一个用于写的句柄
    Q_SOCK addLoger(CLoger *pLoger);

public:
    /*接口实现*/
    void onMainRead(CEventBuffer *pBuffer);
    void onStop(void);

public:
    static void LogerReadCB(struct bufferevent *, void *arg);
    static void timerCB(evutil_socket_t, short, void *arg);

private:
    int setTimer(unsigned int uiMS);

private:
    struct event *m_pTimeEvent;
    CQMutex m_objMutex;
    std::list<LogerInfo *> m_lstLoger;
    CTcpParser m_objTcpParser;
};

#endif//Q_LOG_H_
