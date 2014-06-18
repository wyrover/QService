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

#include "Reg2Lua.h"

void Q_LuaLog(int iLV, const char *pszMsg)
{
    Q_LOG((LOG_LEVEL)iLV, "%s", pszMsg);
}

const char *Q_GetModulPath(void)
{
    return g_acModulPath;
}

const char *Q_GetPathSeparator(void)
{
    return Q_PATH_SEPARATOR;
}

void CReg2Lua::Register(void)
{
    reg_Func();
    reg_Session();
    reg_SessionManager();
}

void CReg2Lua::reg_Func(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .addFunction("Q_GetModulPath", Q_GetModulPath)
        .addFunction("Q_GetPathSeparator", Q_GetPathSeparator)
        .addFunction("Q_LOG", Q_LuaLog);
}

void CReg2Lua::reg_Session(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CSession>("CSession")
            .addFunction("getSessionID", &CSession::getSessionID)
            .addFunction("setAssociatedID", &CSession::setAssociatedID)
            .addFunction("getAssociatedID", &CSession::getAssociatedID)
            .addFunction("setStatus", &CSession::setStatus)
            .addFunction("getStatus", &CSession::getStatus)
        .endClass();
}

void CReg2Lua::reg_SessionManager(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CSessionManager>("CSessionManager")
            .addFunction("getSessionSize", &CSessionManager::getSessionSize)
            .addFunction("closeClintByID", &CSessionManager::closeClintByID)
            .addFunction("closeCurClint", &CSessionManager::closeCurClint)

            .addFunction("getCurSession", &CSessionManager::getCurSession)
            .addFunction("getSessionByID", &CSessionManager::getSessionByID)
            .addFunction("getServerLinkerSession", &CSessionManager::getServerLinkerSession)

            .addFunction("sendToCur", &CSessionManager::sendToCur)
            .addFunction("sendToByID", &CSessionManager::sendToByID)
            .addFunction("sendToAll", &CSessionManager::sendToAll)

            .addFunction("pushMsg", &CSessionManager::pushMsg)
            .addFunction("sendPushMsgToCur", &CSessionManager::sendPushMsgToCur)
            .addFunction("sendPushMsgToByID", &CSessionManager::sendPushMsgToByID)
            .addFunction("sendPushMsgToAll", &CSessionManager::sendPushMsgToAll)
            .addFunction("Clear", &CSessionManager::Clear)

            .addFunction("getTimer", &CSessionManager::getTimer)
            .addFunction("getCount", &CSessionManager::getCount)
            .addFunction("getThreadIndex", &CSessionManager::getThreadIndex)
        .endClass();
}
