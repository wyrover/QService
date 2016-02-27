
#ifndef Q_LUAENCRYPT_H_
#define Q_LUAENCRYPT_H_

#include "QRSA.h"
#include "md5.h"
#include "AES.h"
#include "SHA1.h"
#include "BufferMgr.h"
#include "Singleton.h"

//±à½âÂë
class CEncrypt : public CSingleton<CEncrypt>
{
public:
    CEncrypt(void);
    ~CEncrypt(void);

    //base 64
    std::string b64Encode(std::string strVal);
    std::string b64Decode(std::string strVal);

    //md5
    std::string md5Str(std::string strVal);
    std::string md5File(std::string strFile);

    //sha1
    std::string sha1Str(std::string strVal);
    std::string sha1File(std::string strFile);

    //aes
    void setAESKey(const char *pszKey, unsigned short usKeyType);
    std::string aesEncode(std::string strVal);
    std::string aesDecode(std::string strVal);

    //RSA
    int setRSAKey(const char *pszPubKeyFile, const char *pszPriKeyFile, const char *pszRandKeyFile);
    std::string rsaPubEncode(std::string strVal);
    std::string rsaPriDecode(std::string strVal);

    std::string rsaPriEncode(std::string strVal);
    std::string rsaPubDecode(std::string strVal);

    //url
    std::string urlEncode(std::string strVal); 
    std::string urlDecode(std::string strVal);

    //zlib
    std::string zlibEncode(std::string strVal);
    const char *zlibCompress(const char *pszVal, const size_t &iValLens, size_t &iOutLens);
    std::string zlibDecode(std::string strVal);
    const char *zlibDecompression(const char *pszVal, const size_t &iValLens, size_t &iOutLens);

    CAESEncode *getAESEncode(void)
    {
        return &m_objAESEncode;
    };
    CAESDecode *getAESDecode(void)
    {
        return &m_objAESDecode;
    };
    CRSA *getRSA(void)
    {
        return &m_objRSA;
    };

private:
    unsigned char toHex(unsigned char x);
    unsigned char fromHex(unsigned char x);

private:
    char m_acSha[20];
    CMD5 m_objMD5;
    CSHA1 m_objSha1;
    CAESEncode m_objAESEncode;
    CAESDecode m_objAESDecode;
    CRSAKey m_objRSAKey;
    CRSA m_objRSA;
    CBuffer m_objBuffer;
};

#endif//Q_LUAENCRYPT_H_