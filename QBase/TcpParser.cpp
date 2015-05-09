
#include "TcpParser.h"
#include "EventBuffer.h"

#define TCPBUFLENS_125 125
#define TCPBUFLENS_126 126
#define TCPBUFLENS_127 127

CTcpParser::CTcpParser(void) : m_iParsedLens(Q_INIT_NUMBER), m_iBufLens(Q_INIT_NUMBER),
    m_iHeadLens(Q_INIT_NUMBER), m_iTotalLens(Q_INIT_NUMBER), m_iNeedReadLens(Q_INIT_NUMBER)
{
    Q_Zero(m_acHead, sizeof(m_acHead));
}

CTcpParser::~CTcpParser(void)
{

}

bool CTcpParser::parseHead(class CEventBuffer *pBuffer)
{
    m_iNeedReadLens = 1;
    char *pHead = pBuffer->readBuffer(m_iNeedReadLens);
    if (NULL == pHead)
    {
        return false;
    }

    char cFlag = pHead[0];
    if (cFlag <= TCPBUFLENS_125)
    {
        m_iHeadLens = sizeof(cFlag);
        m_iBufLens = (size_t)cFlag;
    }
    else if (TCPBUFLENS_126 == cFlag)
    {
        m_iNeedReadLens = sizeof(cFlag) + sizeof(unsigned short);
        if (m_iTotalLens < m_iNeedReadLens)
        {
            return false;
        }

        pHead = pBuffer->readBuffer(m_iNeedReadLens);
        if (NULL == pHead)
        {
            return false;
        }

        m_iBufLens = ntohs(*((unsigned short *)(pHead + sizeof(cFlag))));
        m_iHeadLens = sizeof(cFlag) + sizeof(unsigned short);
    }
    else if (TCPBUFLENS_127 == cFlag)
    {
        m_iNeedReadLens = sizeof(cFlag) + sizeof(unsigned int);
        if (m_iTotalLens < m_iNeedReadLens)
        {
            return false;
        }

        pHead = pBuffer->readBuffer(m_iNeedReadLens);
        if (NULL == pHead)
        {
            return false;
        }

        m_iBufLens = ntohl(*((unsigned int *)(pHead + sizeof(cFlag))));
        m_iHeadLens = sizeof(cFlag) + sizeof(unsigned int);
    }
    else
    {
        return false;
    }

    return true;
}

const char *CTcpParser::parsePack(class CEventBuffer *pBuffer)
{
    m_iBufLens = Q_INIT_NUMBER;
    m_iParsedLens = Q_INIT_NUMBER;
    m_iTotalLens = pBuffer->getTotalLens();

    if (0 == m_iTotalLens)
    {
        return NULL;
    }
    
    if (!parseHead(pBuffer))
    {
        return NULL;
    }

    m_iNeedReadLens = m_iHeadLens + m_iBufLens;
    if (m_iNeedReadLens > m_iTotalLens)
    {
        return NULL;
    }

    if (Q_INIT_NUMBER == m_iBufLens)
    {
        m_iParsedLens = m_iHeadLens;

        return "";
    }

    char *pMsg = pBuffer->readBuffer(m_iNeedReadLens);
    if (NULL == pMsg)
    {
        return NULL;
    }

    m_iParsedLens = m_iHeadLens + m_iBufLens;

    return (pMsg + m_iHeadLens);
}

const char *CTcpParser::createHead(const size_t &iLens, size_t &iHeadLens)
{
    iHeadLens = Q_INIT_NUMBER;

    if (iLens <= TCPBUFLENS_125)
    {
        m_acHead[0] = (char)iLens;
        iHeadLens = sizeof(char);
    }
    else if ((iLens > TCPBUFLENS_125) && (iLens <= 0xFFFF))
    {
        m_acHead[0] = TCPBUFLENS_126;
        unsigned short usLen = ntohs((unsigned short)iLens);
        memcpy(m_acHead + 1, &usLen, sizeof(usLen));

        iHeadLens = sizeof(char) + sizeof(usLen);
    }
    else 
    {
        m_acHead[0] = TCPBUFLENS_127;
        unsigned int uiLens = ntohl(iLens);
        memcpy(m_acHead + 1, &uiLens, sizeof(uiLens));

        iHeadLens = sizeof(char) + sizeof(uiLens);
    }

    return m_acHead;
}
