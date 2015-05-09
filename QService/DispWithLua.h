
#ifndef Q_NETEVENT_DISPOSE_H_
#define Q_NETEVENT_DISPOSE_H_

#include "Reg2Lua.h"

class CDisposeEvent : public CEventInterface
{
public:
    CDisposeEvent(const char *pszLuaFile);
    ~CDisposeEvent(void);

    /*工作线程启动时执行*/
    bool onSerciveStartUp(void);
    /*工作线程关闭时执行*/
    void onSerciveShutDown(void);

    /*定时器触发时执行*/
    void onTimerEvent(void);    
    
    /*连接成功*/
    void onConnected(class CSession *pSession);
    /*socket读取到完整包时执行*/
    void onSockRead(const char *pszMsg, const size_t &iLens);
    /*socket断开时执行*/
    void onSockClose(class CSession *pSession);

    /*http */
    void onHttpRead(class CHttpParser *pBuffer);

    /*服务器连接启动*/
    void onLinkedOther(class CSession *pSession);

private:
    CDisposeEvent(void);

private:
    struct lua_State *m_pLua;//lua句柄
    CReg2Lua m_objReg2Lua;
};

#endif//Q_NETEVENT_DISPOSE_H_
