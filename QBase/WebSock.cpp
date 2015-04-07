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

#include "WebSock.h"
#include "SessionManager.h"
#include "EventInterface.h"
#include "QString.h"

#define ShakeHands_EndFlag "\r\n\r\n"
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

CWebSock::CWebSock(void) : m_pSessionMgr(NULL)
{
    m_strVersion = Q_FormatStr("%d-%d-%d", Q_MAJOR, Q_MINOR, Q_RELEASE);
}

CWebSock::~CWebSock(void)
{

}

void CWebSock::setSessionMgr(class CSessionManager *pSessionMgr)
{
    assert(NULL != pSessionMgr);

    m_pSessionMgr = pSessionMgr;
}

std::string CWebSock::parseShakeHands(std::list<std::string> &lstShakeHands)
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

std::string CWebSock::createChallengeKey(std::string &strKey)
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
    
    std::string strCBKey = m_objbase64.Encode(m_acShaKey, sizeof(m_acShaKey));

    return strCBKey;
}

std::string CWebSock::createHandshakeResponse(std::string &strKey)
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

bool CWebSock::shakeHands(bool &bClose)
{
    bClose = false;

    size_t iEndFlagLens = strlen(ShakeHands_EndFlag);
    CSession *pSession = m_pSessionMgr->getCurSession();
    struct evbuffer_ptr stPos = pSession->getBuffer()->Search(ShakeHands_EndFlag, iEndFlagLens);
    if (-1 == stPos.pos)
    {
        return false;
    }

    //读取
    std::string strBuf(pSession->getBuffer()->readBuffer(stPos.pos), stPos.pos);
    m_pSessionMgr->getCurSession()->getBuffer()->delBuffer(stPos.pos + iEndFlagLens);

    //解析
    std::list<std::string> lstTmp;
    Q_Split(strBuf, std::string(ShakeHands_SplitFlag), lstTmp);
    std::string strKey = parseShakeHands(lstTmp);
    if (strKey.empty())
    {
        bClose = true;

        return false;
    }

    //创建返回的key
    strKey = createChallengeKey(strKey);
    std::string strRtn = createHandshakeResponse(strKey);

    pSession->getBuffer()->writeBuffer(strRtn.c_str(), strRtn.size());

    return true;
}

bool CWebSock::parseHead(size_t &iFramLens, bool &bClose, const size_t &iTotalLens)
{
    iFramLens = Q_INIT_NUMBER;
    CSession *pSession = m_pSessionMgr->getCurSession();
    memset(&m_stWebSockFram, 0, sizeof(m_stWebSockFram));

    char *pBuf = pSession->getBuffer()->readBuffer(FRAME_HEAD_BASE_LEN);
    if (NULL == pBuf)
    {
        return false;
    }

    m_stWebSockFram.cFin = (pBuf[0] & 0x80) >> 7;
    m_stWebSockFram.cRsv1 = (pBuf[0] & 0x40) >> 6;
    m_stWebSockFram.cRsv2 = (pBuf[0] & 0x20) >> 5;
    m_stWebSockFram.cRsv3 = (pBuf[0] & 0x10) >> 4;
    m_stWebSockFram.emOpCode = (WebSockOpCode)(pBuf[0] & 0xF);
    m_stWebSockFram.cMask = (pBuf[1] & 0x80) >> 7;
    m_stWebSockFram.ucPayloadLen = pBuf[1] & 0x7F;
    if ((1 != m_stWebSockFram.cMask)
        || (0 != m_stWebSockFram.cRsv1)
        || (0 != m_stWebSockFram.cRsv2)
        || (0 != m_stWebSockFram.cRsv3))
    {
        bClose = true;

        return false;
    }

    if (m_stWebSockFram.ucPayloadLen <= PAYLOADLENS_125)
    {
        iFramLens = FRAME_HEAD_BASE_LEN;

        m_stWebSockFram.acMaskKey[0] = pBuf[2];
        m_stWebSockFram.acMaskKey[1] = pBuf[3];
        m_stWebSockFram.acMaskKey[2] = pBuf[4];
        m_stWebSockFram.acMaskKey[3] = pBuf[5];
        m_stWebSockFram.uiDataLens = m_stWebSockFram.ucPayloadLen;        
    }
    else if (PAYLOADLENS_126 == m_stWebSockFram.ucPayloadLen)
    {
        if (iTotalLens < FRAME_HEAD_EXT16_LEN)
        {
            return false;
        }

        iFramLens = FRAME_HEAD_EXT16_LEN;
        pBuf = pSession->getBuffer()->readBuffer(FRAME_HEAD_EXT16_LEN);
        if (NULL == pBuf)
        {
            return false;
        }

        uint16_t uiLoadLens = 0;
        uiLoadLens |= ((uint8_t)pBuf[2]) << 8;
        uiLoadLens |= ((uint8_t)pBuf[3]) << 0;

        m_stWebSockFram.acMaskKey[0] = pBuf[4];
        m_stWebSockFram.acMaskKey[1] = pBuf[5];
        m_stWebSockFram.acMaskKey[2] = pBuf[6];
        m_stWebSockFram.acMaskKey[3] = pBuf[7];
        m_stWebSockFram.uiDataLens = uiLoadLens;
    }
    else if (PAYLOADLENS_127 == m_stWebSockFram.ucPayloadLen)
    {
        if (iTotalLens < FRAME_HEAD_EXT64_LEN)
        {
            return false;
        }

        iFramLens = FRAME_HEAD_EXT64_LEN;
        pBuf = pSession->getBuffer()->readBuffer(FRAME_HEAD_EXT64_LEN);
        if (NULL == pBuf)
        {
            return false;
        }

        uint64_t uiLoadLens = 0;
        uiLoadLens |= ((uint64_t)pBuf[2]) << 56;
        uiLoadLens |= ((uint64_t)pBuf[3]) << 48;
        uiLoadLens |= ((uint64_t)pBuf[4]) << 40;
        uiLoadLens |= ((uint64_t)pBuf[5]) << 32;
        uiLoadLens |= ((uint64_t)pBuf[6]) << 24;
        uiLoadLens |= ((uint64_t)pBuf[7]) << 16;
        uiLoadLens |= ((uint64_t)pBuf[8]) << 8;
        uiLoadLens |= ((uint64_t)pBuf[9]) << 0;

        m_stWebSockFram.acMaskKey[0] = pBuf[10];
        m_stWebSockFram.acMaskKey[1] = pBuf[11];
        m_stWebSockFram.acMaskKey[2] = pBuf[12];
        m_stWebSockFram.acMaskKey[3] = pBuf[13];
        m_stWebSockFram.uiDataLens = (size_t)uiLoadLens;
    }
    else
    {
        Q_Printf("%s", "invalid payload len.");
        bClose = true;

        return false;
    }

    return true;
}

void CWebSock::parseData(char *pData,  bool &bClose)
{
    CSession *pSession = m_pSessionMgr->getCurSession();

    //控制帧处理
    bool bControl = false;
    switch(m_stWebSockFram.emOpCode)
    {
    case CLOSE:
        {
            bClose = true;
            bControl = true;
        }
        break;

    case PING:
        {
            bControl = true;

            size_t iOutLens = Q_INIT_NUMBER;
            const char *pHead = createWebSockHead(true, PONG, m_stWebSockFram.uiDataLens, iOutLens);
            if (NULL == pHead)
            {
                break;
            }

            pSession->getBuffer()->writeBuffer(pHead, iOutLens);
            pSession->getBuffer()->writeBuffer(pData, m_stWebSockFram.uiDataLens);
        }
        break;

    case PONG:
        {
            bControl = true;
            //nothing to do
        }
        break;

    default:
        break;
    }
    
    if (bControl)
    {
        return;
    }
    
    for (size_t i = 0; i < m_stWebSockFram.uiDataLens; i++)
    {
        pData[i] = pData[i] ^ m_stWebSockFram.acMaskKey[i % 4];
    }

    //完整帧
    if ((CONTINUATION != m_stWebSockFram.emOpCode)
        && (1 == m_stWebSockFram.cFin))
    {
        m_pSessionMgr->getInterface()->onSocketRead(pData, m_stWebSockFram.uiDataLens);
    }
    else//一个分片的消息由起始帧（FIN为0，opcode非0），若干（0个或多个）帧（FIN为0，opcode为0），结束帧（FIN为1，opcode为0）
    {
        Q_SOCK sock = pSession->getBuffer()->getFD();

        //是否为结束帧
        if ((1 == m_stWebSockFram.cFin)
            && (CONTINUATION == m_stWebSockFram.emOpCode))
        {
            std::string *pBuf = getContinuation(sock);
            if (NULL != pBuf)
            {
                pBuf->append(pData, m_stWebSockFram.uiDataLens);
                m_pSessionMgr->getInterface()->onSocketRead(pBuf->c_str(), pBuf->size());
                pBuf->clear();
            }            
        }
        else
        {
            addContinuation(sock, pData, m_stWebSockFram.uiDataLens);
        }
    }
}

void CWebSock::addContinuation(const Q_SOCK &sock, const char *pszData, const size_t &iLens)
{
    std::tr1::unordered_map<Q_SOCK, std::string>::iterator itFram;
    itFram = m_mapFram.find(sock);
    if (m_mapFram.end() == itFram)
    {
        std::string strBuf(pszData, iLens);
        m_mapFram.insert(std::make_pair(sock, strBuf));

        return;
    }

    itFram->second.append(pszData, iLens);
}

std::string *CWebSock::getContinuation(const Q_SOCK &sock)
{
    std::tr1::unordered_map<Q_SOCK, std::string>::iterator itFram;
    itFram = m_mapFram.find(sock);
    if (m_mapFram.end() == itFram)
    {
        return NULL;
    }

    return &(itFram->second);
}

void CWebSock::delContinuation(const Q_SOCK &sock)
{
    std::tr1::unordered_map<Q_SOCK, std::string>::iterator itFram;
    itFram = m_mapFram.find(sock);
    if (m_mapFram.end() == itFram)
    {
        return;
    }

    m_mapFram.erase(itFram);
}

void CWebSock::dispData(bool &bClose)
{
    bool bRtn = false;
    char *pData = NULL;
    size_t iFramLens = Q_INIT_NUMBER;
    size_t iTotalLens = Q_INIT_NUMBER;
    CSession *pSession = m_pSessionMgr->getCurSession();
   
    while(true)
    {
        //是否足够最少的一帧长度
        iTotalLens = pSession->getBuffer()->getTotalLens();
        if (iTotalLens < FRAME_HEAD_BASE_LEN)
        {
            break;
        }

        //解析帧头
        bRtn = parseHead(iFramLens, bClose, iTotalLens);
        if (bClose 
            || !bRtn)
        {
            break;
        }      
        
        //有数据
        if (m_stWebSockFram.uiDataLens > 0)
        {
            //数据不足
            if (iTotalLens < (iFramLens + m_stWebSockFram.uiDataLens))
            {
                break;
            }

            pData = pSession->getBuffer()->readBuffer(iFramLens + m_stWebSockFram.uiDataLens);
            if (NULL == pData)
            {
                break;
            }
        }

        //处理
        parseData(pData + iFramLens, bClose);
        if (bClose)
        {
            break;
        }

        if (SessionStatus_Closed != pSession->getStatus())
        {
            pSession->getBuffer()->delBuffer(iFramLens + m_stWebSockFram.uiDataLens);
        }
        else
        {
            break;
        }
    }
}

const char *CWebSock::createWebSockHead(const bool &bFin, const WebSockOpCode emCode, 
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
    else if (iDataLens > PAYLOADLENS_125 && iDataLens <= 0xffff)
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
