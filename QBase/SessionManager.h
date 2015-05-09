
#ifndef Q_SESSION_MANAGER_H_
#define Q_SESSION_MANAGER_H_

#include "Session.h"
#include "EventInterface.h"
#include "WebSockParser.h"
#include "TcpParser.h"
#include "LinkOther.h"
#include "Binary.h"

/*session管理*/
class CSessionManager
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
    /*增加连接到其他服务器的TCP连接*/
    bool addLinkOther(const char *pszIp, const unsigned short usPort, const char *pszNam);
    /*根据名称获取链接*/
    int getLinkOtherID(const char *pszName);
    /*获取session数量*/
    size_t getSessionSize(void) const;

    /*发送*/
    bool sendToCur(const char *pszData, const size_t uiLens);
    bool sendToByID(const int iID, const char *pszData, const size_t uiLens);
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
    /*设置事件处理对象*/
    void setInterface(class CEventInterface *pInterface);
    class CEventInterface *getInterface(void);
    /*定时器触发时间*/
    void setTimer(const unsigned int &uiMS);
    unsigned int getTimer(void) const;
    /*定时器触发次数*/
    void addCount(void);
    unsigned int getCount(void) const;
    /*设置lua虚拟机*/
    void setLua(struct lua_State *pLua);
    /*设置工作线程指针*/
    void setWorkThread(class CWorkThreadEvent *pThread);

    CLinkOther *getLinkOther(void)
    {
        return &m_objLinkOther;
    };

    CLuaBinary *getBinary(void)
    {
        return &m_objBinary;
    };
private:
    /*释放所有session*/
    void freeAllSession(void);
    bool sendTcp(CSession *pCurrent, const char *pszData, const size_t &uiLens);
    bool sendWebSock(CSession *pCurrent, const char *pszData, const size_t &uiLens);

private:
    unsigned int m_uiTimer;
    unsigned int m_uiCount;
    struct lua_State *m_pLua;
    CSession *m_pCurrent; //当前Session
    class CEventInterface *m_pInterface;
    class CWorkThreadEvent *m_pWorkThread;
    std::tr1::unordered_map<int, CSession *> m_umapSession;//所有Session
    std::queue<CSession *> m_quFreeSession;//空闲的session
    std::tr1::unordered_map<std::string, int> m_umapLinkOther;
    CWebSockParser m_objWebSockParser;
    CTcpParser m_objTcpParser;
    CLinkOther m_objLinkOther;
    CLuaBinary m_objBinary;    
};

#endif//Q_SESSION_MANAGER_H_
