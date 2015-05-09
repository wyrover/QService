
#ifndef Q_LINKOTHER_H_
#define Q_LINKOTHER_H_

#include "InitSock.h"
#include "Mutex.h"

//连接到其他
class CLinkOther
{
public:
    CLinkOther(void);
    ~CLinkOther(void);

    //设置CWorkThreadEvent对象
    void setThreadEvent(class CWorkThreadEvent *pThreadEvent);
    class CWorkThreadEvent *getThreadEvent(void);

    //服务运行状态
    void setStatus(const RunStatus emStatus);
    RunStatus getStatus(void) const;

    //增加要连接的主机信息
    bool addHost(const char *pszIp, unsigned short usPort, const char *pszName);

    //开始、停止
    bool Start(void);
    void Stop(void);

    //连接
    void Link(void);

    //单个连接运行状态
    void setSockStatus(Q_SOCK sock, SessionStatus emStatus);
    
    //根据名称获取链接ID
    int getSockByName(const char *pszName);

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
            emStatus = SessionStatus_Closed;
            sock = Q_INVALID_SOCK;
        };
    };

private:
    Q_SOCK initSock(const char *pszIp, const unsigned short &usPort) const;
    bool checkHaveHost(const char *pszName);

private:
    RunStatus m_emStatus;
    class CWorkThreadEvent *m_pThreadEvent;
    std::vector<LinkInfo> m_vcLinkInfo;
    CQMutex m_objMutex;
};

#endif//Q_LINKOTHER_H_
