
#ifndef Q_RSAKEY_H_
#define Q_RSAKEY_H_

#include "Macros.h"
#include <RSAEuro/rsa.h>

/*RSA算法密码类*/
class CRSAKey
{
public:
    CRSAKey(void);
    ~CRSAKey(void);

    /*生成key(长度512 - 1024)*/
    int creatKey(const unsigned short usKeyLens);

    /*保存到文件，base64编码保存*/
    /*保存R_RANDOM_STRUCT 公钥加密需要使用*/
    int saveRandom(const char *pszFile);
    /*保存公钥*/
    int savePublicKey(const char *pszFile);
    /*保存私钥*/
    int savePrivateKey(const char *pszFile);

    /*读取Key*/
    int loadPublicKey(const char *pszFile);
    int loadPrivateKey(const char *pszFile);
    int loadRandom(const char *pszFile);

    /*返回生成的Key或者从文件读取的Key*/
    R_RSA_PUBLIC_KEY *getPublicKey(void);
    R_RSA_PRIVATE_KEY *getPrivateKey(void);
    R_RANDOM_STRUCT *getRandom(void);

private:
    std::string Decode(std::string &strVal);
    std::string Encode(const char *pszMsg, const size_t &iLens);
    int fileWrite(const char *pszMsg, const size_t iLens, const char *pszFile) const;
    std::string fileRead(const char *pszFile) const;
    int loadInfo(char *pInfo, const size_t iLens, const char *pszFile);

private:
    enum RSAKeyLen
    {
        RSAKeyMinLen = 512,
        RSAKeyMaxLen = 1024
    };

    R_RANDOM_STRUCT m_stRandom;
    R_RSA_PUBLIC_KEY m_stPublicKey;    /* RSA public key */   
    R_RSA_PRIVATE_KEY m_stPrivateKey;  /* RSA private key */
};

#endif//Q_RSAKEY_H_
