
#ifndef Q_MAILSENDER_H_
#define Q_MAILSENDER_H_

#include "CTask.h"
#include "SockPairEvent.h"
#include "Singleton.h"
#include "TcpParser.h"
#include "jwsmtp/mailer.h"

class CMailSender : public CTask,
    public CSockPairEvent,
    public CSingleton<CMailSender>
{
public:
    CMailSender(void);
    ~CMailSender(void);

    //任务接口
    void Run(void)
    {
        Q_Printf("%s", "mail system running...");
        Start();
    };

    //SockPairEvent读接口
    void onMainRead(CEventBuffer *pEventBuffer);

    //设置
    void setServer(const std::string &strServer);
    void setAuthType(const enum jwsmtp::mailer::authtype &emType);
    void setSender(const std::string &strSender);
    void setUserName(const std::string &strUserName);
    void setPassWord(const std::string &strPassWord);

    //发送json格式
    void sendMail(const std::string &strMail);

private:
    int parseMail(std::string &strMail);

private:
    jwsmtp::mailer m_objMailer;
    CTcpParser m_objTcpParser;   
};

#endif//Q_MAILSENDER_H_
