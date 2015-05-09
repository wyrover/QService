
#ifndef Q_QRSA_H_
#define Q_QRSA_H_

#include "RSAKey.h"
#include "BufferMgr.h"

/*
RSA加密解密类，非线程安全
*/
class CRSA
{
public:
    CRSA(void);
    ~CRSA(void);

    /*设置RSA Key*/
    void setKey(CRSAKey *pRSAKey);

    /*公钥加密,需要R_RANDOM_STRUCT结构体*/
    const char * publicKeyEncrypt(const char* pszData, const size_t &iDataLens, size_t &iOutLens);
    /*私钥解密*/
    const char * privateKeyDecrypt(const char* pszData, const size_t &iDataLens, size_t &iOutLens);

    /*私钥加密*/
    const char * privateKeyEncrypt(const char* pszData, const size_t &iDataLens, size_t &iOutLens);
    /*公钥解密*/
    const char * publicKeyDecrypt(const char* pszData, const size_t &iDataLens, size_t &iOutLens);

private:
    /*加密方式*/
    enum RSAEncryptType
    {
        RSAEncryptType_Public = 0,//公钥加密
        RSAEncryptType_Private   //私钥加密
    };

    /*解密方式*/
    enum RSADecryptType
    {
        RSADecryptType_Public = 0,//公钥解密
        RSADecryptType_Private   //私钥解密
    };

private:
    const char *RSAEncrypt(RSAEncryptType emEnType, const char* pszData, 
        const size_t &iDataLens, size_t &iOutLens);
    const char *RSADecrypt(RSADecryptType emEnType, const char* pszData, 
        const size_t &iDataLens, size_t &iOutLens);

private:
    CRSAKey *m_pRSAKey;//key
    unsigned char m_acOutBuf[MAX_RSA_MODULUS_LEN];
    CBuffer m_objBuffer;
};

#endif//Q_QRSA_H_
