
#include "ClientEncrypt.h"
#include "Encrypt.h"

SINGLETON_INIT(CClientEncrypt)
CClientEncrypt objClientEncrypt;

void CClientEncrypt::addType(EncryptType emType)
{
    m_vcType.push_back(emType);
}

const char *CClientEncrypt::Encode(const char *pszMsg, const size_t iMsgLens, size_t &iOutLens)
{
    iOutLens = Q_INIT_NUMBER;

    if (m_vcType.empty())
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
    for (itType = m_vcType.begin(); m_vcType.end() != itType; itType++)
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
                        getRSA()->privateKeyEncrypt(m_objBuffer.getBuffer(), m_objBuffer.getLens(), iTmpLens);
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

const char *CClientEncrypt::Decode(const char *pszMsg, const size_t iMsgLens, size_t &iOutLens)
{
    iOutLens = Q_INIT_NUMBER;

    if (m_vcType.empty())
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
    for (itType = m_vcType.rbegin(); m_vcType.rend() != itType; itType++)
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
                        getRSA()->privateKeyDecrypt(pszMsg, iMsgLens, iTmpLens);
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
                        getRSA()->privateKeyDecrypt(m_objBuffer.getBuffer(), m_objBuffer.getLens(), iTmpLens);
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

