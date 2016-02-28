
#ifndef Q_CLIENTENCRYPT_H_
#define Q_CLIENTENCRYPT_H_

#include "../QBase/QBase.h"

class CClientEncrypt : public CSingleton<CClientEncrypt>
{
public:
    CClientEncrypt(void)
    {

    };
    ~CClientEncrypt(void)
    {

    };

    void addType(EncryptType emType);

    const char *Encode(const char *pszMsg, const size_t iMsgLens, size_t &iOutLens);
    const char *Decode(const char *pszMsg, const size_t iMsgLens, size_t &iOutLens);

private:
    CBuffer m_objBuffer;
    std::vector<EncryptType> m_vcType;
};

#endif//Q_CLIENTENCRYPT_H_
