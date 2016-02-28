
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

    void addType(EncryptType emType, bool bSVLink = false);

    const char *Encode(const char *pszMsg, const size_t iMsgLens, size_t &iOutLens, bool bSVLink = false);
    const char *Decode(const char *pszMsg, const size_t iMsgLens, size_t &iOutLens, bool bSVLink = false);

private:
    CBuffer m_objBuffer;
    std::vector<EncryptType> m_vcClinetType;
    std::vector<EncryptType> m_vcSVLinkType;
};

#endif//Q_COMMENCRYPT_H_
