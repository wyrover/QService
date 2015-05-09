
#ifndef Q_SOCKPAIREVENT_H_
#define Q_SOCKPAIREVENT_H_

#include "SockPair.h"
#include "EventBuffer.h"
#include "Cond.h"

/*
SockPair事件循环
*/
class CSockPairEvent
{
public:
    CSockPairEvent(void);
    virtual ~CSockPairEvent(void);

    /*启动事件循环*/
    int Start(void);
    /*停止事件循环*/
    void Stop(void);
    /*等待进入事件循环*/
    bool waitForStarted(void) const;
    /*状态设置*/
    void setRunStatus(RunStatus emStatus);
    RunStatus getRunStatus(void) const;
    /*onStop是否执行*/
    void setRunOnStop(bool bRun);
    bool getRunOnStop(void) const;

    /*写入数据*/
    int sendMainMsg(const char *pszBuff, const size_t &iSize) const;
    int sendAssistMsg(const char *pszBuff, const size_t &iSize) const;
    /*获取event_base*/
    struct event_base *getBase(void)
    {
        return m_pBase;
    };
    CEventBuffer *getMainBuffer(void)
    {
        return &m_objMainBuffer;
    };
    CEventBuffer *getAssistBuffer(void)
    {
        return &m_objAssistBuffer;
    };

public:
    virtual void onMainRead(CEventBuffer *){};
    virtual void onAssistRead(CEventBuffer *){};
    virtual void onStop(void)
    {
        setRunStatus(RunStatus_Stopped);
    };
    virtual bool onStartUp(void)
    {
        return true;
    };
public:
    static void mainReadCB(struct bufferevent *, void *arg);
    static void assistReadCB(struct bufferevent *, void *arg);
    static void eventCB(struct bufferevent *, short event, void *arg);
    static void exitMonitorCB(evutil_socket_t, short, void *arg);

private:
    void freeAll(void);
    int initMainEvent(void);
    int initAssistEvent(void);
    int initExitMonitor(unsigned int uiMS);
    bool getError(void) const;
    bool getIsRun(void) const;

private:
    bool m_bRunOnStop;
    RunStatus m_emStatus;
    struct event_base *m_pBase;
    struct bufferevent *m_pMainBev;
    struct bufferevent *m_pAssistBev;
    struct event *m_pExitEvent;
    CSockPair m_objMainSockPair;
    CSockPair m_objAssistSockPair;
    CEventBuffer m_objMainBuffer; 
    CEventBuffer m_objAssistBuffer; 
    CQMutex m_Mutex;
    CCond m_Cond;
};

/*从buffer中读取.返回指针需要手动删除*/
template<class T>
T *Q_GetEventValue(CEventBuffer *pBuffer)
{
    size_t iSize = sizeof(T);

    size_t iTotalSize = pBuffer->getTotalLens();
    if (iSize > iTotalSize)
    {
        return NULL;
    }

    T *pDate = new(std::nothrow) T();
    if (NULL == pDate)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

        return NULL;
    }

    char *pTmp = pBuffer->readBuffer(iSize);
    *pDate = *((T *)pTmp);
    (void)pBuffer->delBuffer(iSize);

    return pDate;
}

/*从buffer中读取*/
template<class T>
bool Q_GetEventValue(CEventBuffer *pBuffer, T &Date)
{
    size_t iSize = sizeof(T);

    size_t iTotalSize = pBuffer->getTotalLens();
    if (iSize > iTotalSize)
    {
        return false;
    }

    char *pTmp = pBuffer->readBuffer(iSize);
    Date = *((T*)pTmp);
    (void)pBuffer->delBuffer(iSize);

    return true;
}

#endif//Q_SOCKPAIREVENT_H_
