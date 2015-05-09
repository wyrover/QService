
#ifndef Q_INITSOCK_H_
#define Q_INITSOCK_H_

#include "Macros.h"

/*
windows加载套接字库
*/
class CSockInit
{
public:
    CSockInit(void);
    ~CSockInit(void);

private:
    int Init(void) const;
    void Destroy(void) const;
};

/*从socket中读取内容*/
int Q_SockRead(const Q_SOCK &fd, char *pBuf, const size_t &iLens);

/*向socket中写入发送的内容*/
int Q_SockWrite(const Q_SOCK &fd, const char *pBuf, const size_t &iLens);

#endif//Q_INITSOCK_H_
