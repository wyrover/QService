
#ifndef Q_SOCKPAIR_H_
#define Q_SOCKPAIR_H_

#include "NETAddr.h"

/*
相互连接的2个socket
*/
class CSockPair
{
public:
    CSockPair(void);
    ~CSockPair(void);
    
    /*获取读sock句柄*/
    Q_SOCK getReadFD(void) const
    {
        return m_ReadFD;
    };
    /*获取写sock句柄*/
    Q_SOCK getWriteFD(void) const
    {
        return m_WriteFD;
    };
    
    /*从读句柄读取数据*/
    int Read(char *pBuf, const size_t &iLens) const;
    /*向写句柄写入数据*/
    int Write(const char *pBuf, const size_t &iLens) const;

private:
    int Init(void);
    void Destroy(void);
    int creatSockPair(Q_SOCK aeSock[2]) const;
    int creatListener(Q_SOCK &Listener) const;

private:
    Q_SOCK m_ReadFD;
    Q_SOCK m_WriteFD;
    CSockInit m_sockInit;
};

#endif//Q_SOCKPAIR_H_
