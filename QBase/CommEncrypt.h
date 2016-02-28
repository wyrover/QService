
#ifndef Q_COMMENCRYPT_H_
#define Q_COMMENCRYPT_H_

#include "BufferMgr.h"
#include "Singleton.h"

class CCommEncrypt : public CSingleton<CCommEncrypt>
{
public:
    CCommEncrypt(void)
    {

    };
    ~CCommEncrypt(void)
    {

    };

    void addType(EncryptType emType);

    const char *Encode(const char *pszMsg, const size_t iMsgLens, size_t &iOutLens);
    const char *Decode(const char *pszMsg, const size_t iMsgLens, size_t &iOutLens);

private:
    CBuffer m_objBuffer;
    std::vector<EncryptType> m_vcType;
};

#endif//Q_COMMENCRYPT_H_
