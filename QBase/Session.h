
#ifndef Q_SESSION_H_
#define Q_SESSION_H_

#include "EventBuffer.h"

/*session 信息*/
class CSession
{
public:
    CSession(void);
    ~CSession(void);

    /*session id号*/
    void setSessionID(const Q_SOCK &uiID);
    int getSessionID(void) const;

    /*关联的ID*/
    void setExterID(const char *pszID);
    const char * getExterID(void) const;

    /*标识该session唯一ID 连接成功后会自动设置*/
    void setIDCard(const char *pszID);
    const char * getIDCard(void) const;

    /*ping*/
    void setPing(const unsigned int uiPing);
    unsigned int getPing(void) const;

    /*账号*/
    void setAccount(const char *pszAccount);
    const char *getAccount(void) const;

    /*状态*/
    void setStatus(const int iStatus);
    int getStatus(void) const;

    /*session 类型  enum SessionType*/
    void setType(const SessionType emType);
    int getType(void) const;

    void Clear(void);

    CEventBuffer *getBuffer(void);

private:
    SessionType m_emSessionType;   
    SessionStatus m_emStatus;
    Q_SOCK m_iSessionID;//session id      
    unsigned int m_uiPing;
    std::string m_strID;//关联的ID
    std::string m_strAccount;
    std::string m_strIDCard;
    CEventBuffer m_objBuffer;
};

#endif//Q_SESSION_H_
