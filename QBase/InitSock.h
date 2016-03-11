
#ifndef Q_INITSOCK_H_
#define Q_INITSOCK_H_

#include "Macros.h"

/*
windows�����׽��ֿ�
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

/*��socket�ж�ȡ����*/
int Q_SockRead(const Q_SOCK &fd, char *pBuf, const size_t &iLens);

/*��socket��д�뷢�͵�����*/
int Q_SockWrite(const Q_SOCK &fd, const char *pBuf, const size_t &iLens);

/*TCP keepalive*/
void Q_SockKeepAlive(Q_SOCK &fd, const unsigned int iKeepIdle, const unsigned int iKeepInterval);

#endif//Q_INITSOCK_H_
