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

#include "TcpParser.h"
#include "EventBuffer.h"

#define TCPBUFLENS_125 125
#define TCPBUFLENS_126 126
#define TCPBUFLENS_127 127

CTcpParser::CTcpParser(void) : m_iParsedLens(Q_INIT_NUMBER), m_iBufLens(Q_INIT_NUMBER),
    m_iHeadLens(Q_INIT_NUMBER), m_iTotalLens(Q_INIT_NUMBER), m_iNeedReadLens(Q_INIT_NUMBER)
{

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
        m_iBufLens = cFlag;
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

    if (0 >= m_iTotalLens)
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
        unsigned short usLen = ntohs(iLens);
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
