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

#ifndef Q_SERVICEINIT_H_
#define Q_SERVICEINIT_H_

#include "DispWithLua.h"

class CServerInit
{
public:
    CServerInit(void)
    {};
    ~CServerInit(void);

    int Start(void);
    void Stop(void);

private:
    struct LinkOther
    {
        unsigned short usPort;
        std::string strIp;
        std::string strName;
    };
    struct ServerConfig
    {
        unsigned short usThreadNum;
        unsigned int uiTimer;
        std::string strScript;

        std::map<unsigned short, std::string> mapTcp;
        std::map<unsigned short, std::string> mapWebSock;
        std::map<unsigned short, std::string> mapHttp;
    };

private:
    void initTxtLog(void);
    bool initDBLog(void);
    bool readConfig(void);
    int initServer(void);
    void readLinkOtherConfig(std::vector<LinkOther> &vcLinkOther) const;

private:
    CThread m_objThread;
    std::string m_strConfig;
    xml_document m_objXmlDoc;
    xml_parse_result m_objXmlResult;
    xml_node m_objXmlNode;
    CLog m_objLog;
    ServerConfig m_stServerConfig;
    CServer m_objServer;
    std::vector<CEventInterface * > m_vcInterface;
};

int Service_InitProgram(void);
int Service_Exit(void);
void SigHandEntry(int iSigNum);

#endif//Q_SERVICEINIT_H_
