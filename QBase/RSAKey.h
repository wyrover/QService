/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#ifndef Q_RSAKEY_H_
#define Q_RSAKEY_H_

#include "AES.h"
#include "Base64.h"
#include "RSAEuro/rsa.h"

/*RSA算法密码类*/
class CRSAKey
{
public:
    CRSAKey(void){};
    ~CRSAKey(void){};

    /*生成key(长度512 - 1024)*/
    int creatKey(const unsigned short usKeyLens);

    /*设置AES加密密码*/
    int setAESKey(const char *pszKey);

    /*保存到文件， 先aes加密然后base64编码保存*/
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
    char *Decode(const char *pszMsg, const size_t iLens, size_t &iOutLens);
    int Encode(const char *pszMsg, const size_t iLens, std::string &strBase64);
    int fileWrite(const char *pszMsg, const size_t iLens, const char *pszFile);
    std::string fileRead(const char *pszFile);
    int loadInfo(char *pInfo, const size_t iLens, const char *pszFile);

private:
    enum RSAKeyLen
    {
        RSAKeyMinLen = 512,
        RSAKeyMaxLen = 1024,
    };

    R_RANDOM_STRUCT m_stRandom;
    R_RSA_PUBLIC_KEY m_stPublicKey;    /* RSA public key */   
    R_RSA_PRIVATE_KEY m_stPrivateKey;  /* RSA private key */
    CAESEncode m_objAESEncode;
    CAESDecode m_objAESDecode;
    CBase64 m_objBase64;
};

#endif//Q_RSAKEY_H_
