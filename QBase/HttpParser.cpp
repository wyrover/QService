
#include "HttpParser.h"

CHttpParser::CHttpParser(void) : m_pEventBuf(NULL), m_Req(NULL)
{
    
}

CHttpParser::~CHttpParser(void)
{
    if (NULL != m_pEventBuf)
    {
        evbuffer_free(m_pEventBuf);
        m_pEventBuf = NULL;
    }

    m_Req = NULL;
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
        m_strPostMsg.append((const char *)evbuffer_pullup(pBuf, (ev_ssize_t)iLens), iLens);
        evbuffer_drain(pBuf, iLens);
    }

    const struct evhttp_uri *pUri = evhttp_request_get_evhttp_uri(m_Req);
    const char *pszQuery = evhttp_uri_get_query(pUri);
    m_strQuery = (NULL == pszQuery ? "" : pszQuery);

    return true;
}

const char *CHttpParser::getQuery(void) const
{
    return m_strQuery.c_str();
}

const char *CHttpParser::getPostMsg(void) const
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
