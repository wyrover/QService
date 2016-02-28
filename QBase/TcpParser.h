
#ifndef Q_TCP_PARSER_H_
#define Q_TCP_PARSER_H_

#include "Macros.h"

#define TCP_HRAD_MAXLENS  5

class CTcpParser
{
public:
    CTcpParser(void);
    ~CTcpParser(void);

    //解析包 
    const char *parsePack(class CEventBuffer *pBuffer);
    size_t getParsedLens(void) const
    {
        return m_iParsedLens;
    };
    size_t getBufLens(void) const
    {
        return m_iBufLens;
    };

    //创建头
    const char *createHead(const size_t &iLens, size_t &iHeadLens);

private:
    bool parseHead(class CEventBuffer *pBuffer);

private:
    size_t m_iParsedLens;
    size_t m_iBufLens;
    size_t m_iHeadLens;
    size_t m_iTotalLens;
    size_t m_iNeedReadLens;
    char m_acHead[TCP_HRAD_MAXLENS];
};

#endif//Q_TCP_PARSER_H_
