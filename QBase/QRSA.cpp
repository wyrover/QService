
#include "QRSA.h"

CRSA::CRSA(void) : m_pRSAKey(NULL), m_objBuffer()
{
    Q_Zero(m_acOutBuf, sizeof(m_acOutBuf));
}

CRSA::~CRSA(void)
{
    m_pRSAKey = NULL;
}

void CRSA::setKey(CRSAKey *pRSAKey)
{
    m_pRSAKey = pRSAKey;
}

const char *CRSA::RSAEncrypt(RSAEncryptType emEnType, const char* pszData, 
    const size_t &iDataLens, size_t &iOutLens)
{
    int iRtn = Q_RTN_OK;
    size_t iStep = Q_INIT_NUMBER;
    size_t iBufLens = Q_INIT_NUMBER;
    size_t iTmpSize = Q_INIT_NUMBER;
    unsigned char *pTmp = NULL;

    m_objBuffer.reSet();
    iOutLens = Q_INIT_NUMBER;
    pTmp = (unsigned char*)(pszData);
    if (RSAEncryptType_Public == emEnType)
    {
        if (NULL == m_pRSAKey->getPublicKey()
            || NULL == m_pRSAKey->getRandom())
        {
            return NULL;
        }

        iStep = (m_pRSAKey->getPublicKey()->bits + 7) / 8 - 11;
    }
    else
    {
        if (NULL == m_pRSAKey->getPrivateKey())
        {
            return NULL;
        }

        iStep = (m_pRSAKey->getPrivateKey()->bits + 7) / 8 - 11;
    } 

    for (size_t i = 0; i < iDataLens; i += iStep)
    {
        iBufLens = (((i + iStep) > iDataLens) ? (iDataLens - i) : iStep);
        if (RSAEncryptType_Public == emEnType)
        {
            iRtn = RSAPublicEncrypt(m_acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                m_pRSAKey->getPublicKey(), m_pRSAKey->getRandom());
        }
        else
        {
            iRtn = RSAPrivateEncrypt(m_acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                m_pRSAKey->getPrivateKey());
        }

        if (Q_RTN_OK != iRtn)
        {
            Q_Printf("%s", "Encrypt error.");

            return NULL;
        }

        m_objBuffer.pushBuff(m_acOutBuf, iTmpSize);
    }

    iOutLens = m_objBuffer.getLens();

    return m_objBuffer.getBuffer();
}

/************************************************************************
* Function name:publicKeyEncrypt
* Description  :公钥加密,需要R_RANDOM_STRUCT结构体
* IN           :pszData --要加密的数据 iDataLens --长度
* OUT          :iOutLens --加密后数据长度
* Return       :const char *
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
const char *  CRSA::publicKeyEncrypt(const char* pszData, const size_t &iDataLens, size_t &iOutLens)
{
    return RSAEncrypt(RSAEncryptType_Public, pszData, iDataLens, iOutLens);
}

/************************************************************************
* Function name:privateKeyEncrypt
* Description  :私钥加密
* IN           :pszData --要加密的数据 iDataLens --长度
* OUT          :iOutLens --加密后数据长度
* Return       :const char *
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
const char *  CRSA::privateKeyEncrypt(const char* pszData, const size_t &iDataLens, size_t &iOutLens)
{
    return RSAEncrypt(RSAEncryptType_Private, pszData, iDataLens, iOutLens);
}

const char *CRSA::RSADecrypt(RSADecryptType emEnType, const char* pszData, 
    const size_t &iDataLens, size_t &iOutLens)
{
    int iRtn = Q_RTN_OK;
    size_t iStep = Q_INIT_NUMBER;
    size_t iBufLens = Q_INIT_NUMBER;
    size_t iTmpSize = Q_INIT_NUMBER;
    unsigned char *pTmp = NULL;    

    m_objBuffer.reSet();
    iOutLens = Q_INIT_NUMBER;
    pTmp = (unsigned char*)(pszData);
    if (RSADecryptType_Public == emEnType)
    {
        if (NULL == m_pRSAKey->getPublicKey())
        {
            return NULL;
        }

        iStep = (m_pRSAKey->getPublicKey()->bits + 7) / 8;
    }
    else
    {
        if (NULL == m_pRSAKey->getPrivateKey())
        {
            return NULL;
        }

        iStep = (m_pRSAKey->getPrivateKey()->bits + 7) / 8;
    } 

    for (size_t i = 0; i < iDataLens; i += iStep)
    {
        iBufLens = (((i + iStep) > iDataLens) ? (iDataLens - i) : iStep);
        if (RSADecryptType_Public == emEnType)
        {
            iRtn = RSAPublicDecrypt(m_acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                m_pRSAKey->getPublicKey());
        }
        else
        {
            iRtn = RSAPrivateDecrypt(m_acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                m_pRSAKey->getPrivateKey());
        }

        if (Q_RTN_OK != iRtn)
        {
            Q_Printf("%s", "Decrypt error.");

            return NULL;
        }

        m_objBuffer.pushBuff(m_acOutBuf, iTmpSize);
    }

    iOutLens = m_objBuffer.getLens();

    return m_objBuffer.getBuffer();
}

/************************************************************************
* Function name:publicKeyDecrypt
* Description  :公钥解密
* IN           :pszData --要解密的数据 iDataLens --长度
* OUT          :iOutLens --解密后数据长度
* Return       :const char *
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
const char *  CRSA::publicKeyDecrypt(const char* pszData, const size_t &iDataLens, size_t &iOutLens)
{
    return RSADecrypt(RSADecryptType_Public, pszData, iDataLens, iOutLens);
}

/************************************************************************
* Function name:privateKeyDecrypt
* Description  :私钥解密
* IN           :pszData --要解密的数据 iDataLens --长度
* OUT          :iOutLens --解密后数据长度
* Return       :const char *
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
const char *  CRSA::privateKeyDecrypt(const char* pszData, const size_t &iDataLens, size_t &iOutLens)
{
    return RSADecrypt(RSADecryptType_Private, pszData, iDataLens, iOutLens);
}
