
#ifndef Q_SESSION_MANAGER_H_
#define Q_SESSION_MANAGER_H_

#include "Session.h"
#include "EventInterface.h"
#include "WebSockParser.h"
#include "TcpParser.h"
#include "Binary.h"
#include "Singleton.h"
#include "ObjectPool.h"
#include "LuaBridge/LuaBridge.h"

/*session管理*/
class CSessionManager : public CSingleton<CSessionManager>
{
public:
    CSessionManager(void);
    ~CSessionManager(void);

    /*根据ID关闭连接*/
    void closeLinkByID(const int iID);
    /*关闭当前连接*/
    void closeCurLink(void);
    /*获取当前连接session*/
    CSession *getCurSession(void);
    /*根据ID获取session*/
    CSession *getSessionByID(const int iID);
    /*确认停止服务*/
    void confirmStop(void);

    /*获取session数量*/
    unsigned int getSessionSize(void) const;

    /*增加连接到其他服务器的TCP连接*/
    bool addLinkOther(const char *pszIp, const unsigned short usPort, 
        const char *pszName, const unsigned short usType);
    /*根据名称获取链接*/
    int getLinkOtherID(const char *pszName);
    luabridge::LuaRef getLinkOtherByType(const unsigned short usType);    

    /*发送*/
    bool sendToCur(const char *pszData, const unsigned int uiLens);
    bool sendToByID(const int iID, const char *pszData, const unsigned int uiLens);
    /*发送m_objBinary中的数据  Lua中对应的g_objBinary*/
    bool sendBToCur(void);
    bool sendBToByID(const int iID);

    /*设置当前触发操作的session*/ 
    void setCurSession(CSession *pSession);
    /*删除session*/
    void dellSession(struct bufferevent *bev);
    /*初始化一session*/
    int addSession(struct bufferevent *bev);
    /*获取session*/
    CSession *getSession(struct bufferevent *bev);
    /*事件处理对象*/
    class CEventInterface *getInterface(void);
    /*定时器触发时间*/   
    unsigned int getTimer(void) const;
    /*定时器触发次数*/
    void addCount(void);
    unsigned int getCount(void) const;

    /*需要设置的*/
     void setTimer(const unsigned int &uiMS);
     void setInterface(class CEventInterface *pInterface);
    void setLua(struct lua_State *pLua);

    CLuaBinary *getNetBinary(void)
    {
        return &m_objNetBinary;
    };
    CLuaBinary *getSerializeBinary(void)
    {
        return &m_objSerializeBinary;
    };
    /*释放所有session*/
    void freeAllSession(void);

private:
    bool sendTcp(CSession *pCurrent, const char *pszData, const unsigned int &uiLens);
    bool sendWebSock(CSession *pCurrent, const char *pszData, const unsigned int &uiLens);

private:
    unsigned int m_uiTimer;
    unsigned int m_uiCount;
    struct lua_State *m_pLua;
    CSession *m_pCurrent; //当前Session
    class CEventInterface *m_pInterface;
    std::tr1::unordered_map<Q_SOCK, CSession *> m_umapSession;//所有Session
    CObjPool<CSession> m_objSessionPool;
    CWebSockParser m_objWebSockParser;
    CTcpParser m_objTcpParser;
    CLuaBinary m_objNetBinary;
    CLuaBinary m_objSerializeBinary; 
};

#endif//Q_SESSION_MANAGER_H_
