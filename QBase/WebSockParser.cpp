
#include "WebSockParser.h"
#include "QString.h"
#include "EventBuffer.h"
#include "OSFunc.h"

#define WebSock_ShakeHands_EndFlag "\r\n\r\n"
#define ShakeHands_SplitFlag "\r\n"
#define WebSocket_Version  "13"

#define PAYLOADLENS_125 125
#define PAYLOADLENS_126 126
#define PAYLOADLENS_127 127

CWebSockParser::CWebSockParser(void) : m_bClose(false), m_iParsedLens(Q_INIT_NUMBER),
    m_iHeadLens(Q_INIT_NUMBER), m_iTotalLens(Q_INIT_NUMBER), m_iNeedReadLens(Q_INIT_NUMBER),
    m_pMsg(NULL)
{
    Q_Zero(m_acShaKey, sizeof(m_acShaKey));
    Q_Zero(m_acWebSockHead, sizeof(m_acWebSockHead));
    m_iShakeHandsEndFlagLens = strlen(WebSock_ShakeHands_EndFlag);
    m_strVersion = Q_FormatStr("%d-%d-%d", Q_MAJOR, Q_MINOR, Q_RELEASE);
    m_stPos.pos = -1;
}

CWebSockParser::~CWebSockParser(void)
{
    m_pMsg = NULL;
}

std::string CWebSockParser::parseShakeHands(std::list<std::string> &lstShakeHands) const
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
        Q_Split(*itShakeHands, ":", lstTmp);
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
    m_objSHA1.Update((const unsigned char *)strKey.c_str(), (unsigned int)strKey.size());
    m_objSHA1.Final();

    if (!m_objSHA1.GetHash(m_acShaKey))
    {
        return "";
    }

    return m_objbase64.Encode(m_acShaKey, sizeof(m_acShaKey));
}

std::string CWebSockParser::createHandshakeResponse(const std::string &strKey) const
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

const std::string *CWebSockParser::shakeHands(class CEventBuffer *pBuffer)
{
    m_bClose = false;
    m_iParsedLens = Q_INIT_NUMBER;
    m_strVal.clear();
    
    m_stPos = pBuffer->Search(WebSock_ShakeHands_EndFlag, m_iShakeHandsEndFlagLens);
    if (-1 == m_stPos.pos)
    {
        return NULL;
    }

    m_iNeedReadLens = (size_t)m_stPos.pos + m_iShakeHandsEndFlagLens;
    char *pShakeHands = pBuffer->readBuffer(m_iNeedReadLens);
    if (NULL == pShakeHands)
    {
        return NULL;
    }

    //解析
    std::list<std::string> lstTmp;
    Q_Split(std::string(pShakeHands), ShakeHands_SplitFlag, lstTmp);
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
    m_iParsedLens = (size_t)m_stPos.pos + m_iShakeHandsEndFlagLens;

    return &m_strVal;
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
        m_stFram.uiDataLens = htons(*(u_short *)(pHead + 2));
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

    m_pMsg = NULL;
    if (m_stFram.uiDataLens > 0)
    {
        char *pMsg = pBuffer->readBuffer(m_iNeedReadLens);
        if (NULL == pMsg)
        {
            return false;
        }

        for (size_t i = 0; i < m_stFram.uiDataLens; i++)
        {
            pMsg[i + m_iHeadLens] = pMsg[i + m_iHeadLens] ^ m_stFram.acMaskKey[i % 4];
        }
        
        m_pMsg = pMsg+ m_iHeadLens;
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
        uint16_t uiLen16 = htons((u_short)iDataLens);
        memcpy(m_acWebSockHead + 2, &uiLen16, sizeof(uiLen16));

        iOutLens = FRAME_HEAD_EXT16_LEN - 4;
    }
    else 
    {
        m_acWebSockHead[1] = PAYLOADLENS_127;
        uint64_t uiLens64 = ntohl64((uint64_t)iDataLens);
        memcpy(m_acWebSockHead + 2, &uiLens64, sizeof(uiLens64));

        iOutLens = FRAME_HEAD_EXT64_LEN - 4;
    }

    return m_acWebSockHead;
}
