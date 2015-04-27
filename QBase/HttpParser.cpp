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

#include "HttpParser.h"

CHttpParser::CHttpParser(void) : m_pEventBuf(NULL)
{
    
}

CHttpParser::~CHttpParser(void)
{
    if (NULL != m_pEventBuf)
    {
        evbuffer_free(m_pEventBuf);
        m_pEventBuf = NULL;
    }
}

bool CHttpParser::setHttpRequest(struct evhttp_request *req)
{
    if (NULL != m_pEventBuf)
    {
        evbuffer_free(m_pEventBuf);
        m_pEventBuf = NULL;
    }

    m_strPostMsg.clear();
    m_strQuery.clear();

    m_pEventBuf = evbuffer_new();
    if (NULL == m_pEventBuf)
    {
        Q_Printf("%s", "evbuffer_new error.");
        return false;
    }

    m_Req = req;

    struct evbuffer *pBuf = evhttp_request_get_input_buffer(m_Req);
    size_t iLens = evbuffer_get_length(pBuf);
    if (iLens > 0)
    {
        m_strPostMsg.append((const char *)evbuffer_pullup(pBuf, iLens), iLens);
        evbuffer_drain(pBuf, iLens);
    }

    const struct evhttp_uri *pUri = evhttp_request_get_evhttp_uri(m_Req);
    const char *pszQuery = evhttp_uri_get_query(pUri);
    m_strQuery = (NULL == pszQuery ? "" : pszQuery);

    return true;
}

const char *CHttpParser::getQuery(void)
{
    return m_strQuery.c_str();
}

const char *CHttpParser::getPostMsg(void)
{
    return m_strPostMsg.c_str();
}

void CHttpParser::setReplyContent(const char *pszMsg)
{
    evbuffer_add_printf(m_pEventBuf, "%s", pszMsg);
}

void CHttpParser::Reply(const int iCode, const char *pszReason)
{
    evhttp_send_reply(m_Req, iCode, pszReason, m_pEventBuf);
}
