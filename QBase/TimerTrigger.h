
#ifndef Q_TIMETRIGGER_H_
#define Q_TIMETRIGGER_H_

#include "Cond.h"
#include "InitEventBase.h"
#include "CTask.h"
#include "Singleton.h"
#include "RunStatus.h"

//定时器触发器
class CTimerTrigger : public CTask, 
    public CRunStatus, 
    public CSingleton<CTimerTrigger>
{
public:
    CTimerTrigger(void);
    ~CTimerTrigger(void);

    //任务接口
    void Run(void)
    {
        Q_Printf("%s", "timer trigger service running...");
        Start();
    };

    //设置触发间隔
    int setTime(const unsigned int &uiMS);

    //启动、停止
    int Start(void);
    void Stop(void);

public:
    static void exitMonitorCB(evutil_socket_t, short, void *arg);
    static void timerCB(evutil_socket_t, short, void *arg);

private:
    int initExitMonitor(unsigned int uiMS);

private:
    struct event_base *m_pBase;
    struct event *m_pExitEvent;
    struct event *m_pTimerEvent;
    CInitBase m_objInitBase;
    CQMutex m_objMutex_Exit;
    CCond m_objCond_Exit;
};

#endif//Q_TIMETRIGGER_H_
