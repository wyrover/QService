
#ifndef Q_HTTP_PARSER_H_
#define Q_HTTP_PARSER_H_

#include "Macros.h"

class CHttpParser
{
public:
    CHttpParser(void);
    ~CHttpParser(void);

    bool setHttpRequest(struct evhttp_request *req);

    /* 获取 */
    const char *getQuery(void) const;
    const char *getPostMsg(void) const;

    /* 设置输出数据 */
    void setReplyContent(const char *pszMsg);
    /* 返回 */
    void Reply(const int iCode, const char *pszReason);

private:
    struct evbuffer *m_pEventBuf;
    struct evhttp_request *m_Req;
    std::string m_strPostMsg;
    std::string m_strQuery;
};

#endif//Q_HTTP_PARSER_H_
