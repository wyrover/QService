
#ifndef Q_LINKOTHER_H_
#define Q_LINKOTHER_H_

#include "InitSock.h"
#include "Mutex.h"
#include "Singleton.h"
#include "RunStatus.h"

//���ӵ�����
class CLinkOther : public CRunStatus, 
    public CSingleton<CLinkOther>
{
public:
    CLinkOther(void);
    ~CLinkOther(void);

    //����Ҫ���ӵ�������Ϣ
    bool addHost(const char *pszIp, const unsigned short usPort, 
        const char *pszName);

    //��ʼ��ֹͣ
    int Start(void);
    void Stop(void);

    //����
    void Link(void);

    //������������״̬
    void setSockStatus(Q_SOCK sock, SessionStatus emStatus);
    
    //�������ƻ�ȡ����ID
    Q_SOCK getSockByName(const char *pszName);

private:
    struct LinkInfo
    {
        SessionStatus emStatus;
        unsigned short usPort;
        Q_SOCK sock;
        std::string strIp;
        std::string strName;
        void Clear(void)
        {
            emStatus = SESSSTATUS_CLOSED;
            sock = Q_INVALID_SOCK;
        };
    };

private:
    Q_SOCK initSock(const char *pszIp, const unsigned short &usPort) const;
    bool checkHaveHost(const char *pszName);

private:
    std::vector<LinkInfo> m_vcLinkInfo;
    CQMutex m_objMutex;
};

#endif//Q_LINKOTHER_H_
