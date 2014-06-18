/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 *
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

#ifndef Q_NETADDR_H_
#define Q_NETADDR_H_

#include "InitSock.h"

/*
网络地址
*/
class CNETAddr
{
public:
    CNETAddr(void);
    explicit CNETAddr(bool ipv6);
    ~CNETAddr(void);

    /*清空数据*/
    void Clear(void);

    /*设置sockaddr_in*/
    int setAddr(const char *pszHostName, unsigned short usPort, bool is_ipv6 = false);
    /*设置sockaddr_in*/
    int	setAddr(const struct sockaddr *pAddr);
    /*根据socket句柄获取远端地址信息*/
    int setRemoteAddr(Q_SOCK &fd);
    /*根据socket句柄获取本地地址信息*/
    int setLocalAddr(Q_SOCK &fd);
    /*返回sockaddr句柄*/
    sockaddr *getAddr(void);
    /*获取地址内存长度*/
    size_t getAddrSize(void);
    /*获取IP*/
    std::string getIp(void);
    /*获取端口*/
    unsigned short getPort(void);
    /*是否为ipv4*/
    bool is_ipv4();
    /*是否为ipv6*/
    bool is_ipv6();
    /*ip转数字*/
    unsigned long IpToNumber(const char *pszIp);

private:
    enum
    {
        IPV4,
        IPV6,
    }m_Addr_Type;

    sockaddr_in	m_ipv4;
    sockaddr_in6 m_ipv6;
    CSockInit m_sockInit;
};

#endif//Q_NETADDR_H_
