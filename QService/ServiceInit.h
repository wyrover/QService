
#ifndef Q_SERVICEINIT_H_
#define Q_SERVICEINIT_H_

#include "DispWithLua.h"

struct LinkOther;

int Service_InitProgram(void);
int Service_Exit(void);
void SigHandEntry(int iSigNum);

//�����
int initEncrypt(void);
int initAESKey(void);
int initRSAKey(void);

int initCommEncrypt(void);

//�ʼ����ͷ���
int initMailSender(void);
int readMailConfig(std::string &strMailServer,
    std::string &strSender,
    std::string &strUserName,
    std::string &strPassWord,
    enum jwsmtp::mailer::authtype &emType,
    bool &bOpen);
void stopMailSender(void);

//��־
int initTxtLoger(void);
int initDBLoger(bool &bUseDBLog);
int initLogSystem(void);
void stopLogSystem(void);

//������������
int initServerLinker(void);
int readLinkConfig(std::vector<LinkOther> &vcLinkOther);
void stopServerLinker(void);

//����
int initServer(void);
int readConfig(unsigned int &uiMS, std::string &strLua,
    std::map<unsigned short, std::string> &mapDebug, 
    std::map<unsigned short, std::string> &mapTcp, 
    std::map<unsigned short, std::string> &mapWebSock, 
    std::map<unsigned short, std::string> &mapHttp,
    std::map<unsigned short, std::string> &mapSVLink);
void stopServer(void);

#endif//Q_SERVICEINIT_H_
