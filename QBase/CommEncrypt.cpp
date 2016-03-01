
#include "CommEncrypt.h"
#include "Encrypt.h"

SINGLETON_INIT(CCommEncrypt)
CCommEncrypt objCommEncrypt;

void CCommEncrypt::addType(EncryptType emType, bool bSVLink)
{
    if (bSVLink)
    {
        m_vcSVLinkType.push_back(emType);
    }
    else
    {
        m_vcClinetType.push_back(emType);
    }    
}

const char *CCommEncrypt::Encode(const char *pszMsg, const size_t iMsgLens, size_t &iOutLens, bool bSVLink)
{
    iOutLens = Q_INIT_NUMBER;

    if (m_vcClinetType.empty())
    {
        iOutLens = iMsgLens;

        return pszMsg;
    }

    if (NULL == pszMsg
        || Q_INIT_NUMBER == iMsgLens)
    {
        iOutLens = iMsgLens;

        return pszMsg;
    }

    bool bFirst = true;
    size_t iTmpLens = Q_INIT_NUMBER;
    std::vector<EncryptType>::iterator itType;
    std::vector<EncryptType> *pType = NULL;
    if (bSVLink)
    {
        pType = &m_vcSVLinkType;
    }
    else
    {
        pType = &m_vcClinetType;
    }

    for (itType = pType->begin(); pType->end() != itType; itType++)
    {
        switch(*itType)
        {
        case AES:
            {
                if (bFirst)
                {
                    const char *pszTmp = CEncrypt::getSingletonPtr()->
                        getAESEncode()->Encode(pszMsg, iMsgLens, iTmpLens);
                    if (NULL == pszTmp)
                    {
                        return NULL;
                    }

                    m_objBuffer.reSet();
                    m_objBuffer.pushBuff(pszTmp, iTmpLens);
                }
                else
                {
                    const char *pszTmp = CEncrypt::getSingletonPtr()->
                        getAESEncode()->Encode(m_objBuffer.getBuffer(), m_objBuffer.getLens(), iTmpLens);
                    if (NULL == pszTmp)
                    {
                        return NULL;
                    }

                    m_objBuffer.reSet();
                    m_objBuffer.pushBuff(pszTmp, iTmpLens);
                }
            }
            break;
        case RSA:
            {
                if (bFirst)
                {
                    if (bSVLink)
                    {
                        const char *pszTmp = CEncrypt::getSingletonPtr()->
                            getRSA()->privateKeyEncrypt(pszMsg, iMsgLens, iTmpLens);
                        if (NULL == pszTmp)
                        {
                            return NULL;
                        }

                        m_objBuffer.reSet();
                        m_objBuffer.pushBuff(pszTmp, iTmpLens);
                    }
                    else
                    {
                        const char *pszTmp = CEncrypt::getSingletonPtr()->
                            getRSA()->publicKeyEncrypt(pszMsg, iMsgLens, iTmpLens);
                        if (NULL == pszTmp)
                        {
                            return NULL;
                        }

                        m_objBuffer.reSet();
                        m_objBuffer.pushBuff(pszTmp, iTmpLens);
                    }
                }
                else
                {
                    if (bSVLink)
                    {
                        const char *pszTmp = CEncrypt::getSingletonPtr()->
                            getRSA()->privateKeyEncrypt(m_objBuffer.getBuffer(), m_objBuffer.getLens(), iTmpLens);
                        if (NULL == pszTmp)
                        {
                            return NULL;
                        }

                        m_objBuffer.reSet();
                        m_objBuffer.pushBuff(pszTmp, iTmpLens);
                    }
                    else
                    {
                        const char *pszTmp = CEncrypt::getSingletonPtr()->
                            getRSA()->publicKeyEncrypt(m_objBuffer.getBuffer(), m_objBuffer.getLens(), iTmpLens);
                        if (NULL == pszTmp)
                        {
                            return NULL;
                        }

                        m_objBuffer.reSet();
                        m_objBuffer.pushBuff(pszTmp, iTmpLens);
                    }
                }
            }
            break;
        case ZLib:
            {
                if (bFirst)
                {
                    const char *pszTmp = CEncrypt::getSingletonPtr()->
                        zlibCompress(pszMsg, iMsgLens, iTmpLens);
                    if (NULL == pszTmp)
                    {
                        return NULL;
                    }

                    m_objBuffer.reSet();
                    m_objBuffer.pushBuff(pszTmp, iTmpLens);
                }
                else
                {
                    const char *pszTmp = CEncrypt::getSingletonPtr()->
                        zlibCompress(m_objBuffer.getBuffer(), m_objBuffer.getLens(), iTmpLens);
                    if (NULL == pszTmp)
                    {
                        return NULL;
                    }

                    m_objBuffer.reSet();
                    m_objBuffer.pushBuff(pszTmp, iTmpLens);
                }
            }
            break;
        }

        bFirst = false;
    }

    iOutLens = m_objBuffer.getLens();

    return m_objBuffer.getBuffer();
}

const char *CCommEncrypt::Decode(const char *pszMsg, const size_t iMsgLens, size_t &iOutLens, bool bSVLink)
{
    iOutLens = Q_INIT_NUMBER;

    if (m_vcClinetType.empty())
    {
        iOutLens = iMsgLens;

        return pszMsg;
    }

    if (NULL == pszMsg
        || Q_INIT_NUMBER == iMsgLens)
    {
        iOutLens = iMsgLens;

        return pszMsg;
    }

    bool bFirst = true;
    size_t iTmpLens = Q_INIT_NUMBER;
    std::vector<EncryptType>::reverse_iterator itType;
    std::vector<EncryptType> *pType = NULL;
    if (bSVLink)
    {
        pType = &m_vcSVLinkType;
    }
    else
    {
        pType = &m_vcClinetType;
    }

    for (itType = pType->rbegin(); pType->rend() != itType; itType++)
    {
        switch(*itType)
        {
        case AES:
            {
                if (bFirst)
                {
                    const char *pszTmp = CEncrypt::getSingletonPtr()->
                        getAESDecode()->Decode(pszMsg, iMsgLens, iTmpLens);
                    if (NULL == pszTmp)
                    {
                        return NULL;
                    }

                    m_objBuffer.reSet();
                    m_objBuffer.pushBuff(pszTmp, iTmpLens);
                }
                else
                {
                    const char *pszTmp = CEncrypt::getSingletonPtr()->
                        getAESDecode()->Decode(m_objBuffer.getBuffer(), m_objBuffer.getLens(), iTmpLens);
                    if (NULL == pszTmp)
                    {
                        return NULL;
                    }

                    m_objBuffer.reSet();
                    m_objBuffer.pushBuff(pszTmp, iTmpLens);
                }
            }
            break;
        case RSA:
            {
                if (bFirst)
                {
                    const char *pszTmp = CEncrypt::getSingletonPtr()->
                        getRSA()->publicKeyDecrypt(pszMsg, iMsgLens, iTmpLens);
                    if (NULL == pszTmp)
                    {
                        return NULL;
                    }

                    m_objBuffer.reSet();
                    m_objBuffer.pushBuff(pszTmp, iTmpLens);
                }
                else
                {
                    const char *pszTmp = CEncrypt::getSingletonPtr()->
                        getRSA()->publicKeyDecrypt(m_objBuffer.getBuffer(), m_objBuffer.getLens(), iTmpLens);
                    if (NULL == pszTmp)
                    {
                        return NULL;
                    }

                    m_objBuffer.reSet();
                    m_objBuffer.pushBuff(pszTmp, iTmpLens);
                }
            }
            break;
        case ZLib:
            {
                if (bFirst)
                {
                    const char *pszTmp = CEncrypt::getSingletonPtr()->
                        zlibDecompression(pszMsg, iMsgLens, iTmpLens);
                    if (NULL == pszTmp)
                    {
                        return NULL;
                    }

                    m_objBuffer.reSet();
                    m_objBuffer.pushBuff(pszTmp, iTmpLens);
                }
                else
                {
                    const char *pszTmp = CEncrypt::getSingletonPtr()->
                        zlibDecompression(m_objBuffer.getBuffer(), m_objBuffer.getLens(), iTmpLens);
                    if (NULL == pszTmp)
                    {
                        return NULL;
                    }

                    m_objBuffer.reSet();
                    m_objBuffer.pushBuff(pszTmp, iTmpLens);
                }
            }
            break;
        }

        bFirst = false;
    }

    iOutLens = m_objBuffer.getLens();

    return m_objBuffer.getBuffer();
}
