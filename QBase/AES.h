
#ifndef Q_AES_H_
#define Q_AES_H_

#include "BufferMgr.h"

/*aes加密 非线程安全*/
class CAESEncode
{
public:
    CAESEncode(void);
    ~CAESEncode(void);

    /*设置密码 pszKey 密码， emKeyType 密码类型*/
    int setKey(const char *pszKey, AESKeyType emKeyType);
    /*加密 pszPlaint需要加密的数据，iLens 需要加密数据长度， iOutLes加密后长度*/
    const char *Encode(const char *pszPlaint, const size_t &iLens, size_t &iOutLes);

private:
    enum BlockSize
    {
        AES_BlockSize = 16
    };

private:
    unsigned long *m_pRK;
    unsigned char *m_pKey;    
    int m_iRounds;
    unsigned char m_uacPlain[AES_BlockSize];
    unsigned char m_uacCipher[AES_BlockSize];
    CBuffer m_objBuffer;
};

/*aes解密 非线程安全*/
class CAESDecode
{
public:
    CAESDecode(void);
    ~CAESDecode(void);

    /*设置密码 pszKey 密码， emKeyType 密码类型*/
    int setKey(const char *pszKey, AESKeyType emKeyType);
    /*解密, pszCipher需要解密的数据，iLens 需要解密数据长度， iOutLes解密后长度*/
    const char *Decode(const char *pszCipher, const size_t &iLens, size_t &iOutLes);

private:
    enum BlockSize
    {
        AES_BlockSize = 16
    };

private:
    unsigned long *m_pRK;
    unsigned char *m_pKey;
    int m_iRounds;
    unsigned char m_uacPlain[AES_BlockSize];
    unsigned char m_uacCipher[AES_BlockSize];
    CBuffer m_objBuffer;
};

#endif//Q_AES_H_
