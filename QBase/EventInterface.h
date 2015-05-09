
#ifndef Q_EVENT_INTERFACE_H_ 
#define Q_EVENT_INTERFACE_H_

#include "Macros.h"

/*
事件处理基类
*/
class CEventInterface
{
public:
    CEventInterface(void) : m_pSessionManager(NULL)
    {
    };

    virtual ~CEventInterface(void)
    {
        m_pSessionManager = NULL;
    };

    void setSessionManager(class CSessionManager *pSessionManager)
    {
        m_pSessionManager = pSessionManager;
    };

    class CSessionManager *getSessionManager(void)
    {
        return m_pSessionManager;
    };
    /*工作线程启动时执行*/
    virtual bool onSerciveStartUp(void){return true;};
    /*工作线程关闭时执行*/
    virtual void onSerciveShutDown(void){};

    /*定时器触发时执行*/
    virtual void onTimerEvent(void){};

    /*连接成功*/
    virtual void onConnected(class CSession *){};
    /*tcp读取到完整包时执行*/
    virtual void onSockRead(const char *, const size_t &){};
    /*socket断开时执行*/
    virtual void onSockClose(class CSession *){};  

    /*http */
    virtual void onHttpRead(class CHttpParser *){};

    /*服务器连接启动*/
    virtual void onLinkedOther(class CSession *){};

private:
    class CSessionManager *m_pSessionManager;
};

#endif//Q_EVENT_INTERFACE_H_
