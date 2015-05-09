
#ifndef Q_BASE64_H_
#define Q_BASE64_H_

#include "BufferMgr.h"

/*Base64 编码解码类，Decode非线程安全*/
class CBase64
{
public:
    CBase64(void);
    ~CBase64(void);

    /*base64编码*/
    std::string Encode(const unsigned char *pszData, const size_t &iLens) const;
    /*base64解码*/
    const char *Decode(const char* pszData, const size_t &iLens, size_t &iOutLens);

private:
    CBuffer m_objBuffer;
};

#endif//Q_BASE64_H_
