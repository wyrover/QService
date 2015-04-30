/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 * github:https://github.com/QService/QService
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "SockPair.h"
#include "Exception.h"

CSockPair::CSockPair(void) : m_ReadFD(Q_INVALID_SOCK), m_WriteFD(Q_INVALID_SOCK)
{
    int iRtn = Init();
    if (Q_RTN_OK != iRtn)
    {
        Q_EXCEPTION(iRtn, "%s", "init socket pair error");
    }
}

CSockPair::~CSockPair(void)
{
    try
    {
        Destroy();
    }
    catch (...)
    {
    	
    }    
}

int CSockPair::creatListener(Q_SOCK &Listener) const
{
    CNETAddr objListen_addr;
    int iRtn = Q_RTN_OK;

    iRtn = objListen_addr.setAddr("127.0.0.1", 0);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("set socket addr error. error code %d.", iRtn);

        return iRtn;
    }

    Listener = socket(AF_INET, SOCK_STREAM, 0);
    if (Q_INVALID_SOCK == Listener)
    {
        iRtn = Q_SockError();
        Q_Printf("create socket error. error code %d, message %s ", iRtn, Q_SockError2Str(iRtn));

        return Q_RTN_FAILE;
    }

    if (Q_RTN_FAILE == bind(Listener, objListen_addr.getAddr(), (int)objListen_addr.getAddrSize()))
    {
        iRtn = Q_SockError();
        Q_Printf("bind port error. error code %d, message %s ", iRtn, Q_SockError2Str(iRtn));

        evutil_closesocket(Listener);

        return Q_RTN_FAILE;
    }

    if (Q_RTN_FAILE == listen(Listener, 1))
    {
        iRtn = Q_SockError();
        Q_Printf("listen error. error code %d, message %s ", iRtn, Q_SockError2Str(iRtn));

        evutil_closesocket(Listener);

        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

int CSockPair::creatSockPair(Q_SOCK aeSock[2]) const
{
    Q_SOCK Listener = Q_INVALID_SOCK;
    Q_SOCK Connector = Q_INVALID_SOCK;
    Q_SOCK Acceptor = Q_INVALID_SOCK;
    ev_socklen_t iSize = Q_INIT_NUMBER;
    struct sockaddr_in connect_addr;
    struct sockaddr_in listen_addr;    
    int iKeepAlive = 1;
    int iRtn = Q_RTN_OK;

    if (Q_RTN_OK != creatListener(Listener))
    {
        return Q_RTN_FAILE;
    } 

    Connector = socket(AF_INET, SOCK_STREAM, 0);
    if (Q_INVALID_SOCK == Connector)
    {
        iRtn = Q_SockError();
        Q_Printf("create socket error. error code %d, message %s ", iRtn, Q_SockError2Str(iRtn));
        evutil_closesocket(Listener);

        return Q_RTN_FAILE;
    }

    iSize = sizeof(connect_addr);
    if (Q_RTN_FAILE == getsockname(Listener, (struct sockaddr *) &connect_addr, &iSize))
    {
        iRtn = Q_SockError();
        Q_Printf("getsockname error. error code %d, message %s ", iRtn, Q_SockError2Str(iRtn));

        evutil_closesocket(Listener);
        evutil_closesocket(Connector);

        return Q_RTN_FAILE;
    }

    if (iSize != sizeof (connect_addr))
    {
        Q_Printf("%s", "addr size not equ.");

        evutil_closesocket(Listener);
        evutil_closesocket(Connector);

        return Q_RTN_FAILE;
    }

    if (Q_RTN_FAILE == connect(Connector, (struct sockaddr *) &connect_addr, sizeof(connect_addr)))
    {
        iRtn = Q_SockError();
        Q_Printf("connect error. error code %d, message %s ", iRtn, Q_SockError2Str(iRtn));

        evutil_closesocket(Listener);
        evutil_closesocket(Connector);

        return Q_RTN_FAILE;
    }

    iSize = sizeof(listen_addr);
    Acceptor = accept(Listener, (struct sockaddr *) &listen_addr, &iSize);
    if (Q_INVALID_SOCK == Acceptor)
    {
        iRtn = Q_SockError();
        Q_Printf("accept error. error code %d, message %s ", iRtn, Q_SockError2Str(iRtn));

        evutil_closesocket(Listener);
        evutil_closesocket(Connector);

        return Q_RTN_FAILE;
    }

    evutil_closesocket(Listener);

	if (Q_RTN_FAILE == getsockname(Connector, (struct sockaddr *) &connect_addr, &iSize))
    {
        iRtn = Q_SockError();
        Q_Printf("getsockname error. error code %d, message %s ", iRtn, Q_SockError2Str(iRtn));

        evutil_closesocket(Acceptor);
        evutil_closesocket(Connector);

        return Q_RTN_FAILE;
    }

	if (iSize != sizeof (connect_addr)
		|| listen_addr.sin_family != connect_addr.sin_family
		|| listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr
		|| listen_addr.sin_port != connect_addr.sin_port)
    {
        Q_Printf("%s", "addr not equ.");

        evutil_closesocket(Acceptor);
        evutil_closesocket(Connector);

        return Q_RTN_FAILE;
    }

    (void)setsockopt(Connector, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, (int)sizeof(iKeepAlive));
    (void)setsockopt(Acceptor, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, (int)sizeof(iKeepAlive));
    (void)evutil_make_socket_nonblocking(Acceptor);
    (void)evutil_make_socket_nonblocking(Connector);

	aeSock[0] = Acceptor;
	aeSock[1] = Connector;

    return Q_RTN_OK;
}

int CSockPair::Init(void)
{
    Q_SOCK acSockPair[2] = {Q_INVALID_SOCK, Q_INVALID_SOCK};

    if (Q_RTN_OK != creatSockPair(acSockPair))
    {
        return Q_RTN_FAILE;
    }

    m_ReadFD = acSockPair[0];
    m_WriteFD = acSockPair[1];

    return Q_RTN_OK;
}

/************************************************************************
* Function name:Read
* Description  :从读句柄读取数据
* IN           :pBuf --读取数据存放地 iLens --读取长度
* OUT          :pBuf --读取数据存放地
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int CSockPair::Read(char *pBuf, const size_t &iLens) const
{
    if (NULL == pBuf)
    {
        return Q_ERROR_NULLPOINTER;
    }

    return Q_SockRead(m_ReadFD, pBuf, iLens);
}

/************************************************************************
* Function name:Write
* Description  :向写句柄写入数据
* IN           :pBuf --要写入的数据 iLens --长度
* OUT          :NONE
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int CSockPair::Write(const char *pBuf, const size_t &iLens) const
{
    if (NULL == pBuf)
    {
        return Q_ERROR_NULLPOINTER;
    }

    return Q_SockWrite(m_WriteFD, pBuf, iLens);
}

void CSockPair::Destroy(void)
{
    if (Q_INVALID_SOCK != m_ReadFD)
    {
        evutil_closesocket(m_ReadFD);
        m_ReadFD = Q_INVALID_SOCK;
    }

    if (Q_INVALID_SOCK != m_WriteFD)
    {
        evutil_closesocket(m_WriteFD);
        m_WriteFD = Q_INVALID_SOCK;
    }
}
