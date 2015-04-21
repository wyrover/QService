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

#include "WebSockParser.h"
#include "QString.h"
#include "EventBuffer.h"

#define WebSock_ShakeHands_EndFlag "\r\n\r\n"
#define ShakeHands_SplitFlag "\r\n"
#define WebSocket_Version  "13"

#define PAYLOADLENS_125 125
#define PAYLOADLENS_126 126
#define PAYLOADLENS_127 127

static union  
{
    char a[4];
    unsigned long ul;
}endian = {{'L', '?', '?', 'B'}}; 
#define ENDIAN ((char)endian.ul) 

uint64_t ntohl64(uint64_t host)
{
    if ('L' == ENDIAN)
    {
        uint64_t uiRet = 0;
        unsigned long ulHigh,ulLow;

        ulLow = host & 0xFFFFFFFF;
        ulHigh = (host >> 32) & 0xFFFFFFFF;

        ulLow = ntohl(ulLow); 
        ulHigh = ntohl(ulHigh);

        uiRet = ulLow;
        uiRet <<= 32;
        uiRet |= ulHigh;

        return uiRet;
    }
    else
    {
        return host;
    }
}

CWebSockParser::CWebSockParser(void) : m_bClose(false), m_iParsedLens(Q_INIT_NUMBER),
    m_iHeadLens(Q_INIT_NUMBER), m_iTotalLens(Q_INIT_NUMBER), m_iNeedReadLens(Q_INIT_NUMBER)
{
    m_iShakeHandsEndFlagLens = strlen(WebSock_ShakeHands_EndFlag);
    m_strVersion = Q_FormatStr("%d-%d-%d", Q_MAJOR, Q_MINOR, Q_RELEASE);
    m_strSplitFlag = ShakeHands_SplitFlag;
}

std::string CWebSockParser::parseShakeHands(std::list<std::string> &lstShakeHands)
{
    if (lstShakeHands.empty())
    {
        return "";
    }

    std::list<std::string>::iterator itShakeHands = lstShakeHands.begin();
    if (std::string::npos == itShakeHands->find("GET"))
    {
        return "";
    }
    else
    {
        if ((std::string::npos == itShakeHands->find("HTTP/1.1"))
            && (std::string::npos == itShakeHands->find("HTTP/1.0")))
        {
            return "";
        }
    }

    itShakeHands++;

    bool bUpgrade = false;
    bool bConnection = false;
    bool bKey = false;
    bool bVersion = false; 
    std::string strKey;
    std::string strVal;
    std::string strRtn;
    std::list<std::string> lstTmp;
    std::list<std::string>::iterator itTmp;
    for (; lstShakeHands.end() != itShakeHands; itShakeHands++)
    {
        if (itShakeHands->empty())
        {
            continue;
        }

        lstTmp.clear();
        Q_Split(*itShakeHands, std::string(":"), lstTmp);
        if (2 != lstTmp.size())
        {
            continue;
        }

        itTmp = lstTmp.begin();
        strKey = Q_Trim(*itTmp);
        itTmp++;
        strVal = Q_Trim(*itTmp);
        if (0 == Q_Strcasecmp(strKey.c_str(), "Upgrade"))
        {
            bUpgrade = true;
            if (0 != Q_Strcasecmp(strVal.c_str(), "websocket"))
            {
                return "";
            }
        }
        else if (0 == Q_Strcasecmp(strKey.c_str(), "Connection"))
        {
            bConnection = true;
            if (0 != Q_Strcasecmp(strVal.c_str(), "Upgrade"))
            {
                return "";
            }
        }
        else if (0 == Q_Strcasecmp(strKey.c_str(), "Sec-WebSocket-Key"))
        {
            bKey = true;
            strRtn = strVal;
        }
        else if (0 == Q_Strcasecmp(strKey.c_str(), "Sec-WebSocket-Version"))
        {
            bVersion = true;
            if (0 != Q_Strcasecmp(strVal.c_str(), WebSocket_Version))
            {
                return "";
            }

        }
        else
        {
            continue;
        }
    }

    if (!bUpgrade || !bConnection || !bKey || !bVersion)
    {
        return "";
    }

    return strRtn;
}

std::string CWebSockParser::createChallengeKey(std::string &strKey)
{
    strKey += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    memset(m_acShaKey, 0, sizeof(m_acShaKey));

    m_objSHA1.Reset();
    m_objSHA1.Update((const unsigned char *)strKey.c_str(), strKey.size());
    m_objSHA1.Final();

    if (!m_objSHA1.GetHash(m_acShaKey))
    {
        return "";
    }

    return m_objbase64.Encode(m_acShaKey, sizeof(m_acShaKey));
}

std::string CWebSockParser::createHandshakeResponse(std::string &strKey)
{
    std::string strRtn =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Server: QServer version:" + m_strVersion + "\r\n"
        "Sec-WebSocket-Accept: " + strKey + "\r\n"
        "Sec-WebSocket-Origin: null\r\n"
        "\r\n";

    return strRtn;
}

const char *CWebSockParser::shakeHands(class CEventBuffer *pBuffer)
{
    m_bClose = false;
    m_iParsedLens = Q_INIT_NUMBER;
    m_strVal.clear();
    
    m_stPos = pBuffer->Search(WebSock_ShakeHands_EndFlag, m_iShakeHandsEndFlagLens);
    if (-1 == m_stPos.pos)
    {
        return NULL;
    }

    m_iNeedReadLens = m_stPos.pos + m_iShakeHandsEndFlagLens;
    char *pShakeHands = pBuffer->readBuffer(m_iNeedReadLens);
    if (NULL == pShakeHands)
    {
        return NULL;
    }

    //解析
    std::list<std::string> lstTmp;
    Q_Split(std::string(pShakeHands), m_strSplitFlag, lstTmp);
    m_strVal = parseShakeHands(lstTmp);
    if (m_strVal.empty())
    {
        m_bClose = true;

        return NULL;
    }

    //创建返回的key
    m_strVal = createChallengeKey(m_strVal);
    if (m_strVal.empty())
    {
        m_bClose = true;

        return NULL;
    }

    m_strVal = createHandshakeResponse(m_strVal);
    m_iParsedLens = m_stPos.pos + m_iShakeHandsEndFlagLens;

    return m_strVal.c_str();
}

bool CWebSockParser::parseHead(class CEventBuffer *pBuffer)
{
    m_iHeadLens = Q_INIT_NUMBER;
    m_iNeedReadLens = FRAME_HEAD_BASE_LEN;
    char *pHead = pBuffer->readBuffer(m_iNeedReadLens);
    if (NULL == pHead)
    {
        return false;
    }

    m_stFram.cFin = (pHead[0] & 0x80) >> 7;
    m_stFram.cRsv1 = (pHead[0] & 0x40) >> 6;
    m_stFram.cRsv2 = (pHead[0] & 0x20) >> 5;
    m_stFram.cRsv3 = (pHead[0] & 0x10) >> 4;
    m_stFram.emOpCode = (WebSockOpCode)(pHead[0] & 0xF);
    m_stFram.cMask = (pHead[1] & 0x80) >> 7;
    m_stFram.ucPayloadLen = pHead[1] & 0x7F;
    if ((1 != m_stFram.cMask)
        || (0 != m_stFram.cRsv1)
        || (0 != m_stFram.cRsv2)
        || (0 != m_stFram.cRsv3))
    {
        m_bClose = true;

        return false;
    }

    if (m_stFram.ucPayloadLen <= PAYLOADLENS_125)
    {
        m_iHeadLens = FRAME_HEAD_BASE_LEN;

        m_stFram.acMaskKey[0] = pHead[2];
        m_stFram.acMaskKey[1] = pHead[3];
        m_stFram.acMaskKey[2] = pHead[4];
        m_stFram.acMaskKey[3] = pHead[5];
        m_stFram.uiDataLens = m_stFram.ucPayloadLen;
    }
    else if (PAYLOADLENS_126 == m_stFram.ucPayloadLen)
    {
        if (m_iTotalLens < FRAME_HEAD_EXT16_LEN)
        {
            return false;
        }

        m_iHeadLens = FRAME_HEAD_EXT16_LEN;
        m_iNeedReadLens = m_iHeadLens;
        pHead = pBuffer->readBuffer(m_iNeedReadLens);
        if (NULL == pHead)
        {
            return false;
        }

        m_stFram.acMaskKey[0] = pHead[4];
        m_stFram.acMaskKey[1] = pHead[5];
        m_stFram.acMaskKey[2] = pHead[6];
        m_stFram.acMaskKey[3] = pHead[7];
        m_stFram.uiDataLens = htons(*(unsigned short *)(pHead + 2));
    }
    else if (PAYLOADLENS_127 == m_stFram.ucPayloadLen)
    {
        if (m_iTotalLens < FRAME_HEAD_EXT64_LEN)
        {
            return false;
        }

        m_iHeadLens = FRAME_HEAD_EXT64_LEN;
        m_iNeedReadLens = m_iHeadLens;
        pHead = pBuffer->readBuffer(m_iNeedReadLens);
        if (NULL == pHead)
        {
            return false;
        }

        m_stFram.acMaskKey[0] = pHead[10];
        m_stFram.acMaskKey[1] = pHead[11];
        m_stFram.acMaskKey[2] = pHead[12];
        m_stFram.acMaskKey[3] = pHead[13];
        m_stFram.uiDataLens = (size_t)ntohl64(*(uint64_t *)(pHead + 2));
    }
    else
    {
        Q_Printf("%s", "invalid payload len.");
        m_bClose = true;

        return false;
    }

    return true;
}

bool CWebSockParser::parsePack(class CEventBuffer *pBuffer)
{
    m_iParsedLens = Q_INIT_NUMBER;
    m_bClose = false;
    m_iTotalLens = pBuffer->getTotalLens();
    if (FRAME_HEAD_BASE_LEN > m_iTotalLens)
    {
        return false;
    }

    if (!parseHead(pBuffer))
    {
        return false;
    }

    m_iNeedReadLens = m_iHeadLens + m_stFram.uiDataLens;
    if (m_iTotalLens < m_iNeedReadLens)
    {
        return false;
    }

    if (m_stFram.uiDataLens > 0)
    {
        char *pMsg = pBuffer->readBuffer(m_iNeedReadLens);
        for (size_t i = 0; i < m_stFram.uiDataLens; i++)
        {
            pMsg[i + m_iHeadLens] = pMsg[i + m_iHeadLens] ^ m_stFram.acMaskKey[i % 4];
        }

        m_strVal.clear();
        m_strVal.append(pMsg+ m_iHeadLens, m_stFram.uiDataLens);
    }

    m_iParsedLens = m_iHeadLens + m_stFram.uiDataLens;

    return true;
}

const char *CWebSockParser::createHead(const bool &bFin, const WebSockOpCode emCode, 
    const size_t &iDataLens, size_t &iOutLens)
{
    memset(m_acWebSockHead, 0, sizeof(m_acWebSockHead));

    m_acWebSockHead[0] = (char)(emCode | 0x80);
    if (!bFin)
    {
        m_acWebSockHead[0] = (m_acWebSockHead[0] & 0x7f);
    }

    if (iDataLens <= PAYLOADLENS_125)
    {
        m_acWebSockHead[1] = (char)iDataLens;
        iOutLens = FRAME_HEAD_BASE_LEN - 4;
    }
    else if ((iDataLens > PAYLOADLENS_125) && (iDataLens <= 0xFFFF))
    {
        m_acWebSockHead[1] = PAYLOADLENS_126;
        uint16_t uiLen16 = htons(iDataLens);
        memcpy(m_acWebSockHead + 2, &uiLen16, sizeof(uiLen16));

        iOutLens = FRAME_HEAD_EXT16_LEN - 4;
    }
    else 
    {
        m_acWebSockHead[1] = PAYLOADLENS_127;
        uint64_t uiLens64 = ntohl64(iDataLens);
        memcpy(m_acWebSockHead + 2, &uiLens64, sizeof(uiLens64));

        iOutLens = FRAME_HEAD_EXT64_LEN - 4;
    }

    return m_acWebSockHead;
}
