
#ifndef Q_LINKOTHER_H_
#define Q_LINKOTHER_H_

#include "InitSock.h"
#include "Mutex.h"
#include "Singleton.h"
#include "RunStatus.h"

//连接到其他
class CLinkOther : public CRunStatus, 
    public CSingleton<CLinkOther>
{
public:
    CLinkOther(void);
    ~CLinkOther(void);

    //增加要连接的主机信息
    bool addHost(const char *pszIp, const unsigned short usPort, 
        const char *pszName, const unsigned short usType);

    //开始、停止
    int Start(void);
    void Stop(void);

    //连接
    void Link(void);

    //单个连接运行状态
    void setSockStatus(Q_SOCK sock, SessionStatus emStatus);
    
    //根据名称获取链接ID
    Q_SOCK getSockByName(const char *pszName);
    //根据类型获取
    std::vector<int> getSockByType(const unsigned short usType);

private:
    struct LinkInfo
    {
        SessionStatus emStatus;
        unsigned short usPort;
        unsigned short usType;
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
