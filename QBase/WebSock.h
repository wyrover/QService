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

#ifndef Q_WEBSOCK_H_ 
#define Q_WEBSOCK_H_

#include "Base64.h"
#include "SHA1.h"

#define FRAME_HEAD_BASE_LEN 6
#define FRAME_HEAD_EXT16_LEN 8
#define FRAME_HEAD_EXT64_LEN 14

enum  WebSockOpCode
{
    CONTINUATION = 0x00,
    TEXT_FRAME = 0x01,
    BINARY_FRAME = 0x02,
    CLOSE = 0x08,
    PING = 0x09,
    PONG = 0x0A,
};

class CWebSock
{
public:
    CWebSock(void);
    ~CWebSock(void);

    /*设置session*/
    void setSessionMgr(class CSessionManager *pSessionMgr);

    /*握手处理*/
    bool shakeHands(bool &bClose);
    /*处理数据 返回真则关闭该链接*/
    void dispData(bool &bClose);
    /*删除分片数据帧*/
    void delContinuation(const Q_SOCK &sock);
    /*构造头*/
    const char *createWebSockHead(const bool &bFin, const WebSockOpCode emCode, 
        const size_t &iDataLens, size_t &iOutLens);

private:   

    //websock 解析后的头
    struct WebSockFram
    {
        char cFin;
        char cRsv1;
        char cRsv2;
        char cRsv3;
        WebSockOpCode emOpCode;
        char cMask;
        unsigned char ucPayloadLen;
        size_t uiDataLens;
        char acMaskKey[4];
    };

private:
    std::string parseShakeHands(std::list<std::string> &lstShakeHands);
    std::string createChallengeKey(std::string &strKey);
    std::string createHandshakeResponse(std::string &strKey);
    bool parseHead(size_t &iFramLens, bool &bClose, const size_t &iTotalLens);
     /*处理数据 返回真则关闭该链接*/
    void parseData(char *pData,  bool &bClose);
    void addContinuation(const Q_SOCK &sock, const char *pszData, const size_t &iLens);
    std::string *getContinuation(const Q_SOCK &sock);

private:
    class CSessionManager *m_pSessionMgr;
    CBase64 m_objbase64;
    CSHA1 m_objSHA1;
    unsigned char m_acShaKey[20];
    std::string m_strVersion;
    WebSockFram m_stWebSockFram;
    std::tr1::unordered_map<Q_SOCK, std::string> m_mapFram;
    char m_acWebSockHead[FRAME_HEAD_EXT64_LEN];
};

#endif
