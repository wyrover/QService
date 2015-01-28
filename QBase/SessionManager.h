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

#ifndef Q_SESSION_MANAGER_H_
#define Q_SESSION_MANAGER_H_

#include "Session.h"
#include "Buffer.h"
#include "EventInterface.h"
#include "LuaBridge/LuaBridge.h"

/*session管理*/
class CSessionManager
{
public:
    CSessionManager(void);
    ~CSessionManager(void);

    /*获取session数量*/
    size_t getSessionSize(void);
    /*获取服务器间连接数*/
    int getGetSVLinkerNum(void);
    /*根据ID关闭连接*/
    void closeLinkByID(const int iID);
    /*关闭当前连接*/
    void closeCurLink(void);
    /*获取当前连接session*/
    CSession *getCurSession(void);
    /*根据ID获取session*/
    CSession *getSessionByID(const int iID);
    /*获取服务器间连接的Session*/
    CSession *getServerLinkerSession(const char *pszName);
    /*ping 检查(uiTime 超时时间(ms))*/
    void checkPing(const unsigned int uiTime);

    /*直接发送 格式 unsigned short(消息的长度 + 消息码长度) + 操作码 + 消息*/
    bool sendToCur(const unsigned short usOpCode, const char *pszData, const size_t uiLens);
    bool sendToByID(const int iID, const unsigned short usOpCode, const char *pszData, const size_t uiLens);

    /*设置当前触发操作的session*/ 
    void setCurSession(CSession *pSession);
    /*删除session*/
    void dellSession(struct bufferevent *bev);
    /*初始化一session*/
    int addSession(struct bufferevent *bev);
    /*获取session*/
    CSession *getSession(struct bufferevent *bev);
    /*添加服务器间连接*/
    void addServerLinker(const char *pszName, struct bufferevent *pBufEvent);
    /*删除服务器间连接*/
    void delServerLinker(const char *pszName);
    /*设置事件处理对象*/
    void setInterface(class CEventInterface *pInterface);
    class CEventInterface *getInterface(void);
    /*线程编号*/
    void setThreadIndex(const short &sIndex);
    short getThreadIndex(void);
    /*定时器触发时间*/
    void setTimer(unsigned int &uiMS);
    unsigned int getTimer(void);
    /*定时器触发次数*/
    void addCount(void);
    unsigned int getCount(void);
    /*设置lua虚拟机*/
    void setLua(struct lua_State *pLua);

    /*返回指定类型的服务器连接*/
    luabridge::LuaRef getSVLinkerNameByType(const int iType);
    /*判断是否为指定类型的服务器连接*/
    bool checkType(const int iType, const int iClientID);

private:
    /*释放所有session*/
    void freeAllSession(void);
    bool sendWithHead(CSession *pCurrent, 
        const unsigned short &usOpCode, const char *pszData, const size_t &uiLens);

private:
    short m_sThreadIndex;
    unsigned int m_uiTimer;
    unsigned int m_uiCount;
    struct lua_State *m_pLua;
    CSession *m_pCurrent; //当前Session
    class CEventInterface *m_pInterface;
    std::tr1::unordered_map<int, CSession *> m_unmapSession;//所有Session
    std::queue<CSession *> m_quFreeSession;//空闲的session
    std::tr1::unordered_map<std::string, bufferevent* > m_mapServerLinker;
};

#endif//Q_SESSION_MANAGER_H_
