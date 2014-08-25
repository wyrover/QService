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
    int getSessionID(void);

    /*关联的ID*/
    void setID(const char *pszID);
    const char * getID(void);

    /*是否为连接到其他服务器的*/
    bool getServerLinker(void);

    /*校验ID*/
    void setCheckID(const char *pszID);
    const char * getCheckID(void);

    void Clear(void);

    CEventBuffer *getBuffer(void);
    
    void setServerLinker(bool bLinker);
    void setHandle(void *pHandle);
    void *getHandle(void);

private:    
    bool m_bLinker;
    void *m_pHandle;
    int m_iSessionID;//session id  
    std::string m_strID;//关联的ID
    std::string m_strCheckID;//校验ID
    CEventBuffer m_objBuffer;
};

#endif//Q_SESSION_H_
