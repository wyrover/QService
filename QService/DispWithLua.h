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

#ifndef Q_NETEVENT_DISPOSE_H_
#define Q_NETEVENT_DISPOSE_H_

#include "Reg2Lua.h"

class CDisposeEvent : public CEventInterface
{
public:
    CDisposeEvent(const char *pszLuaFile);
    ~CDisposeEvent(void);

    /*工作线程启动时执行*/
    void onSerciveStartUp(void);
    /*工作线程关闭时执行*/
    void onSerciveShutDown(void);
    /*socket断开时执行*/
    void onSocketClose(void);
    /*定时器触发时执行*/
    void onTimerEvent(void);
    /*socket读取到完整包时执行*/
    void onSocketRead(const char *pszMsg, const Q_PackHeadType &iLens);
    /*服务器连接启动*/
    void onLinkedServer(class CSession *pSession);

private:
    CDisposeEvent(void);

private:
    unsigned short m_usOpCode;
    unsigned short m_usMsgLens;
    luabridge::BinaryStr m_stBinaryStr;
    struct lua_State *m_pLua;//lua句柄
    CReg2Lua m_objReg2Lua;
};

#endif//Q_NETEVENT_DISPOSE_H_
