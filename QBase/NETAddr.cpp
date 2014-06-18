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

#include "NETAddr.h"

CNETAddr::CNETAddr(void) : m_Addr_Type(IPV4)
{
    Clear();
}

CNETAddr::CNETAddr(bool ipv6)
{
    m_Addr_Type = (ipv6 ? IPV6 : IPV4);
    Clear();
}

CNETAddr::~CNETAddr(void)
{

}

void CNETAddr::Clear(void)
{
    Q_Zero(&m_ipv4, sizeof(m_ipv4));
    Q_Zero(&m_ipv6, sizeof(m_ipv6));

    m_ipv4.sin_family = AF_INET;
    m_ipv6.sin6_family = AF_INET6;
}

/************************************************************************
* Function name:setAddr
* Description  :设置sockaddr_in
* IN           :pszHostName --主机名(NULL 0.0.0.0 localhost:127.0.0.1) 
                usPort --端口 is_ipv6 --是否为IPV6
* OUT          :NONE
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int CNETAddr::setAddr(const char *pszHostName, unsigned short usPort, bool is_ipv6)
{
    struct addrinfo stAddrInfo = {0};
    struct addrinfo *pAddrInfo = NULL;
    int  iRtn = Q_RTN_OK;

    Clear();

    if (!is_ipv6)
    {
        m_Addr_Type = IPV4;
        stAddrInfo.ai_flags  = AI_PASSIVE;
        stAddrInfo.ai_family = AF_INET;
    }
    else
    {
        m_Addr_Type = IPV6;
        stAddrInfo.ai_flags  = AI_PASSIVE;
        stAddrInfo.ai_family = AF_INET6;
    }

    iRtn = getaddrinfo(pszHostName, NULL, &stAddrInfo, &pAddrInfo);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("getaddrinfo error. return code %d, message %s.", iRtn, gai_strerror(iRtn));

        if (NULL != pAddrInfo)
        {
            freeaddrinfo(pAddrInfo);
        }

        return iRtn;
    }

    if (AF_INET == pAddrInfo->ai_family)
    {
        memcpy(&m_ipv4, pAddrInfo->ai_addr, pAddrInfo->ai_addrlen);
        m_ipv4.sin_port = htons(usPort);
    }
    else
    {
        memcpy(&m_ipv6, pAddrInfo->ai_addr, pAddrInfo->ai_addrlen);
        m_ipv6.sin6_port = htons(usPort);
    }

    freeaddrinfo(pAddrInfo);

    return Q_RTN_OK;
}

/************************************************************************
* Function name:setAddr
* Description  :设置sockaddr_in
* IN           :pAddr -- sockaddr *
* OUT          :NONE
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int	CNETAddr::setAddr(const struct sockaddr *pAddr)
{
    if (NULL == pAddr)
    {
        return Q_ERROR_NULLPOINTER;
    }

    Clear();

    if (AF_INET == pAddr->sa_family)
    {
        m_Addr_Type = IPV4;
        memcpy(&m_ipv4, pAddr, sizeof(m_ipv4));
    }
    else
    {
        m_Addr_Type = IPV6;
        memcpy(&m_ipv6, pAddr, sizeof(m_ipv6));
    }

    return Q_RTN_OK;
}

/************************************************************************
* Function name:setRemoteAddr
* Description  :根据socket句柄获取远端地址信息
* IN           :fd -- socket句柄
* OUT          :NONE
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int CNETAddr::setRemoteAddr(Q_SOCK &fd)
{
    if (Q_INVALID_SOCK == fd)
    {
        return Q_RTN_FAILE;
    }

    sockaddr_storage stSockAddrStor;
    int iSockStorLens = sizeof(sockaddr_storage);
    Q_Zero(&stSockAddrStor, sizeof(sockaddr_storage));

    Clear();

    int iRtn = getpeername(fd, (sockaddr*)&stSockAddrStor, (socklen_t*) &iSockStorLens);
    if (Q_RTN_OK != iRtn)
    {
        iRtn = Q_SockError();
        Q_Printf("getpeername error. error code %d, message %s", iRtn, Q_SockError2Str(iRtn));

        return iRtn;
    }

    return setAddr((sockaddr*)&stSockAddrStor);
}

/************************************************************************
* Function name:setLocalAddr
* Description  :根据socket句柄获取本地地址信息
* IN           :fd -- socket句柄
* OUT          :NONE
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int CNETAddr::setLocalAddr(Q_SOCK &fd)
{
    sockaddr_storage stSockAddrStor;
    int iSockAddrStorLen = sizeof(sockaddr_storage);
    Q_Zero(&stSockAddrStor, sizeof(sockaddr_storage));

    Clear();

    int iRtn  = getsockname(fd, (sockaddr*)&stSockAddrStor, (socklen_t*)&iSockAddrStorLen);
    if (Q_RTN_OK != iRtn)
    {
        iRtn = Q_SockError();
        Q_Printf("getsockname error. error code %d, message %s", iRtn, Q_SockError2Str(iRtn));

        return iRtn;
    }

    return setAddr((sockaddr*)&stSockAddrStor);
}

/************************************************************************
* Function name:getAddr
* Description  :返回sockaddr句柄
* IN           :NONE
* OUT          :NONE
* Return       :sockaddr*
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
sockaddr *CNETAddr::getAddr(void)
{
    if (IPV4 == m_Addr_Type)
    {
        return (sockaddr*)&m_ipv4;
    }
    else
    {
        return (sockaddr*)&m_ipv6;
    }
}

/************************************************************************
* Function name:getAddrSize
* Description  :获取地址内存长度
* IN           :NONE
* OUT          :NONE
* Return       :size_t
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
size_t CNETAddr::getAddrSize(void)
{
    if (IPV4 == m_Addr_Type)
    {
        return sizeof(m_ipv4);
    }
    else
    {
        return sizeof(m_ipv6);
    }
}

/************************************************************************
* Function name:getIp
* Description  :获取IP
* IN           :NONE
* OUT          :NONE
* Return       :std::string
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
std::string CNETAddr::getIp(void)
{
    int iRtn = Q_RTN_OK;
    char acTmp[200] = {0};
    int iLen = sizeof(acTmp);

    if (IPV4 == m_Addr_Type)
    {
        iRtn = getnameinfo((sockaddr*)&m_ipv4, sizeof(m_ipv4), acTmp, iLen, NULL, 0, NI_NUMERICHOST);
    }
    else
    {
        iRtn = getnameinfo((sockaddr*)&m_ipv6, sizeof(m_ipv6), acTmp, iLen, NULL, 0, NI_NUMERICHOST);
    }

    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("getnameinfo error. return code %d, message %s", iRtn, gai_strerror(iRtn));

        return std::string("");
    }

    return std::string(acTmp);
}

/************************************************************************
* Function name:getPort
* Description  :获取端口
* IN           :NONE
* OUT          :NONE
* Return       :std::string
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
unsigned short CNETAddr::getPort(void)
{
    if (IPV4 == m_Addr_Type)
    {
        return ntohs(m_ipv4.sin_port);
    }
    else
    {
        return ntohs(m_ipv6.sin6_port);
    }
}

bool CNETAddr::is_ipv4()
{
    return (IPV4 == m_Addr_Type);
}

bool CNETAddr::is_ipv6()
{
    return (IPV6 == m_Addr_Type);
}

/************************************************************************
* Function name:IpToNumber
* Description  :ip转数字
* IN           :pszIp --ip
* OUT          :NONE
* Return       :unsigned long
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
unsigned long CNETAddr::IpToNumber(const char *pszIp)
{
    return inet_addr(pszIp);
}
