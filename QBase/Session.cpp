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

#include "Session.h"

CSession::CSession(void) : m_cSessionType(STYPE_TCP),
    m_iSessionID(Q_INVALID_ID), m_iStatus(SessionStatus_Closed), 
    m_uiPing(Q_INIT_NUMBER)
{
    (void)m_objBuffer.setBuffer(NULL);
}

CSession::~CSession(void)
{

}

void CSession::setType(const SessionType emType)
{
    m_cSessionType = (char)emType;
}

int CSession::getType(void)
{
    return (int)m_cSessionType;
}

void CSession::setSessionID(const int &uiID)
{
    m_iSessionID = uiID;
}

int CSession::getSessionID(void)
{
    return m_iSessionID;
}

void CSession::setID(const char *pszID)
{
    assert(NULL != pszID);

    m_strID = pszID;
}

const char * CSession::getID(void)
{
    return m_strID.c_str();
}

void CSession::setCheckID(const char *pszID)
{
    assert(NULL != pszID);

    m_strCheckID = pszID;
}

const char * CSession::getCheckID(void)
{
    return m_strCheckID.c_str();
}

void CSession::setPing(const unsigned int uiPing)
{
    m_uiPing = uiPing;
}

unsigned int CSession::getPing(void)
{
    return m_uiPing;
}

void CSession::setAccount(const char *pszAccount)
{
    assert(NULL != pszAccount);
    m_strAccount = pszAccount;
}

const char *CSession::getAccount(void)
{
    return m_strAccount.c_str();
}

void CSession::setStatus(const int iStatus)
{
    m_iStatus = iStatus;
}

int CSession::getStatus(void)
{
    return m_iStatus;
}

CEventBuffer *CSession::getBuffer(void)
{
    return &m_objBuffer;
}

void CSession::Clear(void)
{
    m_cSessionType = STYPE_TCP;
    m_iSessionID = Q_INVALID_ID;
    m_iStatus = SessionStatus_Closed;
    m_uiPing = Q_INIT_NUMBER;
    m_strID.clear();
    m_strCheckID.clear();
    m_strAccount.clear();
    (void)m_objBuffer.setBuffer(NULL);
}
