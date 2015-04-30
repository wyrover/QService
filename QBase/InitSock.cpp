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

#include "InitSock.h"

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
#ifdef Q_OS_WIN32
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
#ifdef Q_OS_WIN32    
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
