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

#ifndef Q_SERVERLINKER_H_
#define Q_SERVERLINKER_H_

#include "Cond.h"
#include "InitSock.h"
#include "SockPairEvent.h"

/*连接到其他服务器*/
class CServerLinker
{
public:
    CServerLinker(void);
    ~CServerLinker(void);

    /*连接状况*/
    void setLinked(bool bLinked);
    bool getLinked(void);
    /*端口*/
    void setPort(unsigned short usPort);
    unsigned short getPort(void);
    /*IP*/
    void setIp(const char *pszIp);
    const char *getIp(void);
    /*连接类型*/
    void setType(const int iType);
    int getType(void);
    /*该连接事件循环存放点*/
    void setSockPairEvent(CSockPairEvent *pEvent);
    /*开启该连接的监控*/
    int Monitor(void);
    /*连接*/
    int Link(void);
    /*将该连接放入事件循环*/
    int addInLoop(void);
    /*关闭*/
    void closeSock(void);
    /*设置非阻塞*/
    void setNonblock(void);
    /*获取该连接的sock句柄*/
    Q_SOCK getSock(void);
    /*等待退出*/
    CMutex *getCloseMutex(void);
    CCond *getCloseCond(void);
    /*向主线程发送信号*/
    CMutex *getMutex(void);
    CCond *getCond(void);
    /*退出标志*/
    bool getStop(void);
    /*监控线程运行状况*/
    bool getMonitorRun(void);
    void setMonitorRun(bool bMonitorRun);
    /*连接名称*/
    void setLinkerName(const char *pszName);
    const char *getLinkerName(void);

private:
    void setTimeOut(const unsigned int uiMS);

private:
    bool m_bMonitorRun;
    bool m_bStop;
    bool m_bLinked;            //是否连接
    unsigned short m_usDesPort;//连接目标主机端口
    int m_iType;
    Q_SOCK m_Sock;
    CSockPairEvent *m_pEvent;
    std::string m_strDesIp;  //连接目标主机IP
    CMutex m_objMutex_Close;
    CCond m_objCond_Close;
    CMutex m_objMutex;
    CCond m_objCond;
    CSockInit m_objSockInit;
    std::string m_strLinkerName;
};

#endif//Q_SERVERLINKER_H_
