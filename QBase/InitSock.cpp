
#include "InitSock.h"

#define Q_TCPKEEPCOUNT    5

CSockInit::CSockInit(void)
{
    (void)Init();
}

CSockInit::~CSockInit(void)
{
    try
    {
        Destroy();
    }
    catch(...)
    {

    }
}

int CSockInit::Init(void) const
{
#ifdef Q_OS_WIN
    WORD wVersionReq;
    WSAData wsData;

    wVersionReq = MAKEWORD(2, 2);

    int iRtn = WSAStartup(wVersionReq, &wsData);
    if (Q_RTN_OK != iRtn)
    {
        iRtn = Q_SockError();
        Q_Printf("WSAStartup error. error code %d, message %s", iRtn, Q_SockError2Str(iRtn));

        return iRtn;
    }
#endif

    return Q_RTN_OK;
}

void CSockInit::Destroy(void) const
{
#ifdef Q_OS_WIN    
    (void)WSACleanup();
#endif
}

/************************************************************************
* Function name:Q_SockRead
* Description  :从socket中读取内容
* IN           :fd --socket句柄 iLens --读取长度
* OUT          :pBuf --读取数据存放地
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int Q_SockRead(const Q_SOCK &fd, char *pBuf, const size_t &iLens)
{
    int iRecvSize = Q_INIT_NUMBER;
    size_t iRecvTotalSize = Q_INIT_NUMBER;

    do 
    {
        iRecvSize = recv(fd, pBuf + iRecvTotalSize, (int)(iLens - iRecvTotalSize), 0);
        if (iRecvSize <= 0)
        {
            int iRtn = Q_SockError();
            Q_Printf("recv error. error code %d, message %s ", iRtn, Q_SockError2Str(iRtn));

            return Q_RTN_FAILE;
        }

        iRecvTotalSize += (size_t)iRecvSize;

    } while (iRecvTotalSize < iLens);

    return Q_RTN_OK;
}

/************************************************************************
* Function name:Q_SockWrite
* Description  :向socket中写入发送的内容
* IN           :fd --socket句柄 pBuf --要发送的数据 iLens --数据长度
* OUT          :NONE
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int Q_SockWrite(const Q_SOCK &fd, const char *pBuf, const size_t &iLens)
{
    int iSendSize = Q_INIT_NUMBER;
    size_t iSendTotalSize = Q_INIT_NUMBER;

    do 
    {
        iSendSize = send(fd, pBuf + iSendTotalSize, (int)(iLens - iSendTotalSize), 0);
        if (iSendSize <= 0)
        {
            int iRtn = Q_SockError();
            Q_Printf("send error. error code %d, message %s ", iRtn, Q_SockError2Str(iRtn));

            return Q_RTN_FAILE;
        }

        iSendTotalSize += (size_t)iSendSize;

    } while (iLens > iSendTotalSize);

    return Q_RTN_OK;
}

/************************************************************************
* Function name:Q_SockKeepAlive
* Description  :TCP keepalive
* IN           :fd --socket句柄 iKeepIdle --开始首次KeepAlive探测前的TCP空闭时间（秒）
                iKeepInterval --两次KeepAlive探测的时间间隔（秒）  
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2016/03/10
* Modification 
* ......record :first program
************************************************************************/
void Q_SockKeepAlive(Q_SOCK &fd, const unsigned int iKeepIdle, const unsigned int iKeepInterval)
{
    int iKeepAlive = 1;

    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, (int)sizeof(iKeepAlive)) < 0)
    {
        Q_Printf("%s", "setsockopt SO_KEEPALIVE error!");
        return;
    }

#ifdef Q_OS_WIN
    unsigned long ulBytesReturn = Q_INIT_NUMBER;
    tcp_keepalive stAliveIn = {0};
    tcp_keepalive stAliveOut = {0};

    stAliveIn.onoff = 1;
    stAliveIn.keepalivetime = iKeepIdle * 1000;
    stAliveIn.keepaliveinterval = iKeepInterval * 1000;
    if (SOCKET_ERROR == WSAIoctl(fd, SIO_KEEPALIVE_VALS, &stAliveIn, sizeof(stAliveIn), 
        &stAliveOut, sizeof(stAliveOut), &ulBytesReturn, NULL, NULL))
    {
        Q_Printf("%s", "WSAIoctl set error!");
    }
#else 
    int ikeepCount = Q_TCPKEEPCOUNT;//判定断开前的KeepAlive探测次数

    if(setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (void *)&iKeepIdle, sizeof(iKeepIdle)) < 0)  
    {
        Q_Printf("%s", "setsockopt TCP_KEEPIDLE error!");
        return;
    }  
    if(setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (void *)&iKeepInterval, sizeof(iKeepInterval)) < 0)  
    {
        Q_Printf("%s", "setsockopt TCP_KEEPINTVL error!");
        return;
    }  
    if(setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (void *)&ikeepCount, sizeof(ikeepCount)) < 0)  
    {
        Q_Printf("%s", "setsockopt TCP_KEEPCNT error!");
        return;
    }
#endif
}
