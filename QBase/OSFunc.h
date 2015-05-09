
#ifndef Q_HOST_H_
#define Q_HOST_H_

#include "InitSock.h"

/*获取本机主机名*/
std::string Q_HostName(void);
/*执行脚本*/
int Q_ExecCmd(const char *pszCmd, const unsigned int uimSec, 
    std::list<std::string> *lstRst);
/*uint64_t 转网络字节序*/
uint64_t ntohl64(uint64_t host);

#endif//Q_HOST_H_
