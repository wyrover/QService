/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 * github:https://github.com/QService/QService
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

#ifndef Q_QRSA_H_
#define Q_QRSA_H_

#include "RSAKey.h"
#include "Buffer.h"

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
    const char * publicKeyEncrypt(const char* pszData, const size_t iDataLens, size_t &iOutLens);
    /*私钥解密*/
    const char * privateKeyDecrypt(const char* pszData, const size_t iDataLens, size_t &iOutLens);

    /*私钥加密*/
    const char * privateKeyEncrypt(const char* pszData, const size_t iDataLens, size_t &iOutLens);
    /*公钥解密*/
    const char * publicKeyDecrypt(const char* pszData, const size_t iDataLens, size_t &iOutLens);

private:
    /*加密方式*/
    enum RSAEncryptType
    {
        RSAEncryptType_Public = 0,//公钥加密
        RSAEncryptType_Private,   //私钥加密
    };

    /*解密方式*/
    enum RSADecryptType
    {
        RSADecryptType_Public = 0,//公钥解密
        RSADecryptType_Private,   //私钥解密
    };

private:
    const char *RSAEncrypt(RSAEncryptType emEnType, const char* pszData, 
        const size_t iDataLens, size_t &iOutLens);
    const char *RSADecrypt(RSADecryptType emEnType, const char* pszData, 
        const size_t iDataLens, size_t &iOutLens);

private:
    CRSAKey *m_pRSAKey;//key
    unsigned char m_acOutBuf[MAX_RSA_MODULUS_LEN];
    CBuffer m_objBuffer;
};

#endif//Q_QRSA_H_
