
#ifndef Q_SERVICEINIT_H_
#define Q_SERVICEINIT_H_

#include "DispWithLua.h"

struct LinkOther;

int Service_InitProgram(void);
int Service_Exit(void);
void SigHandEntry(int iSigNum);

//编解码
int initEncrypt(void);
int initAESKey(void);
int initRSAKey(void);

//邮件发送服务
int initMailSender(void);
int readMailConfig(std::string &strMailServer,
    std::string &strSender,
    std::string &strUserName,
    std::string &strPassWord,
    enum jwsmtp::mailer::authtype &emType,
    bool &bOpen);
void stopMailSender(void);

//日志
int initTxtLoger(void);
int initDBLoger(bool &bUseDBLog);
int initLogSystem(void);
void stopLogSystem(void);

//服务器间连接
int initServerLinker(void);
int readLinkConfig(std::vector<LinkOther> &vcLinkOther);
void stopServerLinker(void);

//服务
int initServer(void);
int readConfig(unsigned int &uiMS, std::string &strLua,
    std::map<unsigned short, std::string> &mapDebug, 
    std::map<unsigned short, std::string> &mapTcp, 
    std::map<unsigned short, std::string> &mapWebSock, 
    std::map<unsigned short, std::string> &mapHttp);
void stopServer(void);

#endif//Q_SERVICEINIT_H_
