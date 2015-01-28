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

int getServerID(void)
{
    return g_iServerID;
}

int getServerType(void)
{
    return g_iServerType;
}

void CReg2Lua::Register(void)
{
    reg_Func();
    reg_Timer();
    reg_SnowflakeID();
    reg_Filter();
    reg_Charset();
    reg_Ini();
    reg_TableFile();
    reg_Session();
    reg_SessionManager();
    reg_HttpClient();
}

void CReg2Lua::reg_Func(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .addFunction("Q_GetModulPath", Q_GetModulPath)
        .addFunction("Q_GetPathSeparator", Q_GetPathSeparator)
        .addFunction("Q_LOG", Q_LuaLog)
        .addFunction("Q_Sleep", Q_Sleep)
        .addFunction("getServerID", getServerID)
        .addFunction("getServerType", getServerType);
}

void CReg2Lua::reg_Timer(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CTimer>("CTimer")
        .addConstructor<void (*) (void)>()

        .addFunction("reStart", &CTimer::reStart)
        .addFunction("Elapsed", &CTimer::Elapsed)
        .endClass();
}

void CReg2Lua::reg_SnowflakeID(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CSnowflakeID>("CSnowflakeID")
            .addConstructor<void (*) (void)>()

            .addFunction("setMachineID", &CSnowflakeID::setMachineID)
            .addFunction("getSnowflakeID", &CSnowflakeID::getSnowflakeID)
        .endClass();
}

void CReg2Lua::reg_Filter(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CFilter>("CFilter")
            .addConstructor<void (*) (void)>()

            .addFunction("Clear", &CFilter::Clear)

            .addFunction("addSensitiveWord", &CFilter::addSensitiveWord)

            .addFunction("checkHave", &CFilter::checkHave)
            .addFunction("Filter", &CFilter::Filter)
        .endClass();
}

void CReg2Lua::reg_Charset(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CCharset>("CCharset")
            .addConstructor<void (*) (void)>()

            .addFunction("getStrCharset", &CCharset::getStrCharset)
        .endClass();
}

void CReg2Lua::reg_Ini(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CIniFile>("CIniFile")
            .addConstructor<void (*) (void)>()

            .addFunction("setFile", &CIniFile::setFile)

            .addFunction("getStringValue", &CIniFile::getStringValue)
            .addFunction("getIntValue", &CIniFile::getIntValue)
            .addFunction("getFloatValue", &CIniFile::getFloatValue)
        .endClass();
}

void CReg2Lua::reg_TableFile(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CTableFile>("CTableFile")
            .addConstructor<void (*) (void)>()

            .addFunction("setFile", &CTableFile::setFile)
            .addFunction("setSplitFlag", &CTableFile::setSplitFlag)

            .addFunction("Parse", &CTableFile::Parse)
            .addFunction("eof", &CTableFile::eof)
            .addFunction("nextRow", &CTableFile::nextRow)
            .addFunction("reSet", &CTableFile::reSet)

            .addFunction("getStringValue", &CTableFile::getStringValue)
            .addFunction("getIntValue", &CTableFile::getIntValue)
            .addFunction("getFloatValue", &CTableFile::getFloatValue)
        .endClass();
}

void CReg2Lua::reg_Session(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CSession>("CSession")
            .addFunction("getSessionID", &CSession::getSessionID)

            .addFunction("setID", &CSession::setID)
            .addFunction("getID", &CSession::getID)

            .addFunction("setCheckID", &CSession::setCheckID)
            .addFunction("getCheckID", &CSession::getCheckID)

            .addFunction("setPing", &CSession::setPing)

            .addFunction("setAccount", &CSession::setAccount)
            .addFunction("getAccount", &CSession::getAccount)

            .addFunction("setStatus", &CSession::setStatus)
            .addFunction("getStatus", &CSession::getStatus)

            .addFunction("isServerLinker", &CSession::getServerLinker)
        .endClass();
}

void CReg2Lua::reg_SessionManager(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CSessionManager>("CSessionManager")
            .addFunction("getSessionSize", &CSessionManager::getSessionSize)
            .addFunction("getGetSVLinkerNum", &CSessionManager::getGetSVLinkerNum)
            .addFunction("closeLinkByID", &CSessionManager::closeLinkByID)
            .addFunction("closeCurLink", &CSessionManager::closeCurLink)

            .addFunction("getCurSession", &CSessionManager::getCurSession)
            .addFunction("getSessionByID", &CSessionManager::getSessionByID)
            .addFunction("checkType", &CSessionManager::checkType)
            .addFunction("getSVLinkerNameByType", &CSessionManager::getSVLinkerNameByType)
            .addFunction("getServerLinkerSession", &CSessionManager::getServerLinkerSession)

            .addFunction("sendToCur", &CSessionManager::sendToCur)
            .addFunction("sendToByID", &CSessionManager::sendToByID)

            .addFunction("checkPing", &CSessionManager::checkPing)

            .addFunction("getTimer", &CSessionManager::getTimer)
            .addFunction("getCount", &CSessionManager::getCount)
        .endClass();
}

void CReg2Lua::reg_HttpClient(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CHttpClient>("CHttpClient")
            .addConstructor<void (*) (void)>()

            .addFunction("Post", &CHttpClient::Post)
            .addFunction("Get", &CHttpClient::Get)
            .addFunction("Posts", &CHttpClient::Posts)
            .addFunction("Gets", &CHttpClient::Gets)
        .endClass();
}
