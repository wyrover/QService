
#ifndef Q_WEBSOCK_PARSER_H_
#define Q_WEBSOCK_PARSER_H_

#include "Base64.h"
#include "SHA1.h"

#define FRAME_HEAD_BASE_LEN 6
#define FRAME_HEAD_EXT16_LEN 8
#define FRAME_HEAD_EXT64_LEN 14

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
    WebSockFram(void) : cFin(Q_INIT_NUMBER), cRsv1(Q_INIT_NUMBER),
        cRsv2(Q_INIT_NUMBER), cRsv3(Q_INIT_NUMBER), emOpCode(CONTINUATION),
        cMask(Q_INIT_NUMBER), ucPayloadLen(Q_INIT_NUMBER), uiDataLens(Q_INIT_NUMBER)
    {
        Q_Zero(acMaskKey, sizeof(acMaskKey));
    };
};

class CWebSockParser
{
public:
    CWebSockParser(void);
    ~CWebSockParser(void);

    //解析握手
    const std::string *shakeHands(class CEventBuffer *pBuffer);

    //解包
    bool parsePack(class CEventBuffer *pBuffer);
    bool getClose(void) const
    {
        return m_bClose;
    };
    size_t getParsedLens(void) const
    {
        return m_iParsedLens;
    };
    const char *getMsg(void) const
    {
        if (NULL == m_pMsg)
        {
            return "";
        }

        return m_pMsg;
    };
    const WebSockFram *getHead(void) const
    {
        return &m_stFram;
    };

    //创建包头
    const char *createHead(const bool &bFin, const WebSockOpCode emCode, 
        const size_t &iDataLens, size_t &iOutLens);

private:
    std::string parseShakeHands(std::list<std::string> &lstShakeHands) const;
    std::string createChallengeKey(std::string &strKey);
    std::string createHandshakeResponse(const std::string &strKey) const;
    bool parseHead(class CEventBuffer *pBuffer);

private:
    bool m_bClose;
    size_t m_iParsedLens;
    size_t m_iHeadLens;
    size_t m_iTotalLens;
    size_t m_iNeedReadLens;
    size_t m_iShakeHandsEndFlagLens;
    char *m_pMsg;
    unsigned char m_acShaKey[20];
    char m_acWebSockHead[FRAME_HEAD_EXT64_LEN];
    std::string m_strVersion;
    std::string m_strVal;
    CBase64 m_objbase64;
    CSHA1 m_objSHA1;
    WebSockFram m_stFram;
    struct evbuffer_ptr m_stPos;
};

#endif//Q_WEBSOCK_PARSER_H_
