
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

/*session����*/
class CSessionManager : public CSingleton<CSessionManager>
{
public:
    CSessionManager(void);
    ~CSessionManager(void);

    /*����ID�ر�����*/
    void closeLinkByID(const int iID);
    /*�رյ�ǰ����*/
    void closeCurLink(void);
    /*��ȡ��ǰ����session*/
    CSession *getCurSession(void);
    /*����ID��ȡsession*/
    CSession *getSessionByID(const int iID);
    /*ȷ��ֹͣ����*/
    void confirmStop(void);

    /*��ȡsession����*/
    unsigned int getSessionSize(void) const;

    /*�������ӵ�������������TCP����*/
    bool addLinkOther(const char *pszIp, const unsigned short usPort, 
        const char *pszName);
    /*�������ƻ�ȡ����*/
    int getLinkOtherID(const char *pszName);  

    /*����*/
    bool sendToCur(const char *pszData, const unsigned int uiLens);
    bool sendToByID(const int iID, const char *pszData, const unsigned int uiLens);
    /*����m_objBinary�е�����  Lua�ж�Ӧ��g_objBinary*/
    bool sendBToCur(void);
    bool sendBToByID(const int iID);

    /*���õ�ǰ����������session*/ 
    void setCurSession(CSession *pSession);
    /*ɾ��session*/
    void dellSession(struct bufferevent *bev);
    /*��ʼ��һsession*/
    int addSession(struct bufferevent *bev);
    /*��ȡsession*/
    CSession *getSession(struct bufferevent *bev);
    /*�¼��������*/
    class CEventInterface *getInterface(void);
    /*��ʱ������ʱ��*/   
    unsigned int getTimer(void) const;
    /*��ʱ����������*/
    void addCount(void);
    unsigned int getCount(void) const;

    /*��Ҫ���õ�*/
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
    /*�ͷ�����session*/
    void freeAllSession(void);

private:
    bool sendTcp(CSession *pCurrent, const char *pszData, const unsigned int &uiLens);
    bool sendWebSock(CSession *pCurrent, const char *pszData, const unsigned int &uiLens);

private:
    unsigned int m_uiTimer;
    unsigned int m_uiCount;
    struct lua_State *m_pLua;
    CSession *m_pCurrent; //��ǰSession
    class CEventInterface *m_pInterface;
    std::tr1::unordered_map<Q_SOCK, CSession *> m_umapSession;//����Session
    CObjPool<CSession> m_objSessionPool;
    CWebSockParser m_objWebSockParser;
    CTcpParser m_objTcpParser;
    CLuaBinary m_objNetBinary;
    CLuaBinary m_objSerializeBinary; 
};

#endif//Q_SESSION_MANAGER_H_
