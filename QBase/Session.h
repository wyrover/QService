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

#ifndef Q_SESSION_H_
#define Q_SESSION_H_

#include "EventBuffer.h"

/*session 信息*/
class CSession
{
public:
    CSession(void);
    ~CSession(void);

    /*session id号*/
    void setSessionID(const int &uiID);
    int getSessionID(void) const;

    /*关联的ID*/
    void setExterID(const char *pszID);
    const char * getExterID(void) const;

    /*校验ID*/
    void setCheckID(const char *pszID);
    const char * getCheckID(void) const;

    /*标识该session唯一ID*/
    void setIDCard(const char *pszID);
    const char * getIDCard(void) const;

    /*ping*/
    void setPing(const unsigned int uiPing);
    unsigned int getPing(void) const;

    /*账号*/
    void setAccount(const char *pszAccount);
    const char *getAccount(void) const;

    /*状态*/
    void setStatus(const int iStatus);
    int getStatus(void) const;

    /*session 类型  enum SessionType*/
    void setType(const SessionType emType);
    int getType(void) const;

    void Clear(void);

    CEventBuffer *getBuffer(void);

private:
    SessionType m_emSessionType;   
    SessionStatus m_emStatus;
    int m_iSessionID;//session id      
    unsigned int m_uiPing;
    std::string m_strID;//关联的ID
    std::string m_strCheckID;//校验ID
    std::string m_strAccount;
    std::string m_strIDCard;
    CEventBuffer m_objBuffer;
};

#endif//Q_SESSION_H_
