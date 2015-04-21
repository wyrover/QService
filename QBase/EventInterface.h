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

#ifndef Q_EVENT_INTERFACE_H_ 
#define Q_EVENT_INTERFACE_H_

#include "Macros.h"

/*
事件处理基类
*/
class CEventInterface
{
public:
    CEventInterface(void) : m_pSessionManager(NULL)
    {
    };

    virtual ~CEventInterface(void)
    {
    };

    void setSessionManager(class CSessionManager *pSessionManager)
    {
        m_pSessionManager = pSessionManager;
    };

    class CSessionManager *getSessionManager(void)
    {
        return m_pSessionManager;
    };
    /*工作线程启动时执行*/
    virtual bool onSerciveStartUp(void){return true;};
    /*工作线程关闭时执行*/
    virtual void onSerciveShutDown(void){};
    /*socket断开时执行*/
    virtual void onSocketClose(void){};
    /*定时器触发时执行*/
    virtual void onTimerEvent(void){};
    /*tcp读取到完整包时执行*/
    virtual void onTcpRead(const char *, const size_t &){};
    /*websock 读取到完整包时执行*/
    virtual void onWebSockRead(const char *, const size_t &){};
    /*http */
    virtual void onHttpRead(class CHttpParser *){};
    /*服务器连接启动*/
    virtual void onLinkedServer(class CSession *){};

private:
    class CSessionManager *m_pSessionManager;
};

#endif//Q_EVENT_INTERFACE_H_
