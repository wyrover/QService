
#include "Encrypt.h"
#include "Base64.h"
#include "Exception.h"
#include "zlib/zutil.h"
#include "zlib/zlib.h"

SINGLETON_INIT(CEncrypt)
CEncrypt objLuaEncrypt;

CEncrypt::CEncrypt(void)
{

}

CEncrypt::~CEncrypt(void)
{
    
}

std::string CEncrypt::b64Encode(std::string strVal)
{
    const unsigned char *pszVal = (const unsigned char *)strVal.c_str();
    size_t iLens = strVal.size();

    return CBase64::Encode(pszVal, iLens);
}

std::string CEncrypt::b64Decode(std::string strVal)
{
    return CBase64::Decode(strVal);
}

std::string CEncrypt::md5Str(std::string strVal)
{
    m_objMD5.reset();
    m_objMD5.update(strVal);

    return m_objMD5.toString();
}

std::string CEncrypt::md5File(std::string strFile)
{
    m_objMD5.reset();
    m_objMD5.updatefile(strFile.c_str());

    return m_objMD5.toString();
}

std::string CEncrypt::sha1Str(std::string strVal)
{
    const unsigned char *pszVal = (const unsigned char *)strVal.c_str();
    unsigned int uiLens = (unsigned int)strVal.size();

    m_objSha1.Reset();
    m_objSha1.Update(pszVal, uiLens);
    m_objSha1.Final();
    (void)m_objSha1.GetHash((unsigned char*)m_acSha);

    return std::string(m_acSha);
}

std::string CEncrypt::sha1File(std::string strFile)
{
    m_objSha1.Reset();
    m_objSha1.HashFile(strFile.c_str());
    m_objSha1.Final();
    (void)m_objSha1.GetHash((unsigned char*)m_acSha);

    return std::string(m_acSha);
}

void CEncrypt::setAESKey(const char *pszKey, unsigned short usKeyType)
{
    (void)m_objAESEncode.setKey(pszKey, (AESKeyType)usKeyType);
    (void)m_objAESDecode.setKey(pszKey, (AESKeyType)usKeyType);
}

std::string CEncrypt::aesEncode(std::string strVal)
{
    const char *pszVal = strVal.c_str();
    size_t iBufLens = strVal.size();
    size_t iOutLens = Q_INIT_NUMBER;

    const char *pszRst = m_objAESEncode.Encode(pszVal, iBufLens, iOutLens);
    if (NULL == pszRst)
    {
        return "";
    }

    return std::string(pszRst, iOutLens);
}

std::string CEncrypt::aesDecode(std::string strVal)
{
    const char *pszVal = strVal.c_str();
    size_t iBufLens = strVal.size();
    size_t iOutLens = Q_INIT_NUMBER;

    const char *pszRst = m_objAESDecode.Decode(pszVal, iBufLens, iOutLens);
    if (NULL == pszRst)
    {
        return "";
    }

    return std::string(pszRst, iOutLens);
}

int CEncrypt::setRSAKey(const char *pszPubKeyFile, const char *pszPriKeyFile, const char *pszRandKeyFile)
{
    if ((NULL != pszPubKeyFile)
        && (0 != strlen(pszPubKeyFile)))
    {
        if (Q_RTN_OK != m_objRSAKey.loadPublicKey(pszPubKeyFile))
        {
            Q_Printf("load rsa public key from file %s error.", pszPubKeyFile);

            return Q_RTN_FAILE;
        }
    }

    if ((NULL != pszPriKeyFile)
        && (0 != strlen(pszPriKeyFile)))
    {
        if (Q_RTN_OK != m_objRSAKey.loadPrivateKey(pszPriKeyFile))
        {
            Q_Printf("load rsa private key from file %s error.", pszPriKeyFile);

            return Q_RTN_FAILE;
        } 
    }

    if ((NULL != pszRandKeyFile)
        && (0 != strlen(pszRandKeyFile)))
    {
        if (Q_RTN_OK != m_objRSAKey.loadRandom(pszRandKeyFile))
        {
            Q_Printf("load rsa random key from file %s error.", pszRandKeyFile);

            return Q_RTN_FAILE;
        } 
    }

    m_objRSA.setKey(&m_objRSAKey);

    return Q_RTN_OK;
}

std::string CEncrypt::rsaPubEncode(std::string strVal)
{
    const char *pszVal = strVal.c_str();
    size_t iBufLens = strVal.size();
    size_t iOutLens = Q_INIT_NUMBER;

    const char *pszRtn = m_objRSA.publicKeyEncrypt(pszVal, iBufLens, iOutLens);
    if (NULL == pszRtn)
    {
        return "";
    }

    return std::string(pszRtn, iOutLens);
}

std::string CEncrypt::rsaPriDecode(std::string strVal)
{
    const char *pszVal = strVal.c_str();
    size_t iBufLens = strVal.size();
    size_t iOutLens = Q_INIT_NUMBER;

    const char *pszRtn = m_objRSA.privateKeyDecrypt(pszVal, iBufLens, iOutLens);
    if (NULL == pszRtn)
    {
        return "";
    }

    return std::string(pszRtn, iOutLens);
}

std::string CEncrypt::rsaPriEncode(std::string strVal)
{
    const char *pszVal = strVal.c_str();
    size_t iBufLens = strVal.size();
    size_t iOutLens = Q_INIT_NUMBER;

    const char *pszRtn = m_objRSA.privateKeyEncrypt(pszVal, iBufLens, iOutLens);
    if (NULL == pszRtn)
    {
        return "";
    }

    return std::string(pszRtn, iOutLens);
}

std::string CEncrypt::rsaPubDecode(std::string strVal)
{
    const char *pszVal = strVal.c_str();
    size_t iBufLens = strVal.size();
    size_t iOutLens = Q_INIT_NUMBER;

    const char *pszRtn = m_objRSA.publicKeyDecrypt(pszVal, iBufLens, iOutLens);
    if (NULL == pszRtn)
    {
        return "";
    }

    return std::string(pszRtn, iOutLens);
}

unsigned char CEncrypt::toHex(unsigned char x)
{
    return  ((x > 9) ? (x + 55) : (x + 48));
}

unsigned char CEncrypt::fromHex(unsigned char x)
{
    unsigned char y;  

    if (x >= 'A' && x <= 'Z') 
    {
        y = x - 'A' + 10;  
    }
    else if (x >= 'a' && x <= 'z') 
    {
        y = x - 'a' + 10;  
    }
    else if (x >= '0' && x <= '9') 
    {
        y = x - '0';  
    }
    else 
    {
        assert(false);  
    }

    return y;  
}

std::string CEncrypt::urlEncode(std::string strVal)
{
    const char *pszVal = strVal.c_str();
    std::string strTemp = "";  
    size_t iLens = strVal.size();

    for (size_t i = 0; i < iLens; i++)  
    {  
        if (isalnum((unsigned char)pszVal[i]) 
            || ('-' == pszVal[i]) 
            || ('_' == pszVal[i]) 
            || ('.' == pszVal[i]) 
            || ('~' == pszVal[i]))
        {
            strTemp += pszVal[i]; 
        }
        else if (' ' == pszVal[i])
        {
            strTemp += "+";  
        }
        else  
        {  
            strTemp += '%';  
            strTemp += toHex((unsigned char)pszVal[i] >> 4);  
            strTemp += toHex((unsigned char)pszVal[i] % 16);  
        }  
    }

    return strTemp; 
}

std::string CEncrypt::urlDecode(std::string strVal)
{
    const char *pszVal = strVal.c_str();
    std::string strTemp = "";
    size_t iLens = strVal.size();  

    for (size_t i = 0; i < iLens; i++)  
    {  
        if ('+' == pszVal[i]) 
        {
            strTemp += ' ';  
        }
        else if ('%' == pszVal[i])  
        {
            assert(i + 2 < iLens);

            unsigned char cHigh = fromHex((unsigned char)pszVal[++i]);  
            unsigned char cLow = fromHex((unsigned char)pszVal[++i]); 

            strTemp += cHigh*16 + cLow;
        }  
        else 
        {
            strTemp += pszVal[i];  
        }
    }

    return strTemp;  
}

const char *CEncrypt::zlibCompress(const char *pszVal, const size_t &iValLens, size_t &iOutLens)
{
    char *pBuffer = NULL;
    uLong ulEnSize = compressBound((uLong)iValLens);
    iOutLens = Q_INIT_NUMBER;

    pBuffer = m_objBuffer.New(ulEnSize);
    if (NULL == pBuffer)
    {
        return NULL;
    }

    if (Z_OK != compress((Bytef*)pBuffer, &ulEnSize, (Bytef*)pszVal, (uLong)iValLens))
    {
        return NULL;
    }

    iOutLens = ulEnSize;

    return pBuffer;
}

std::string CEncrypt::zlibEncode(std::string strVal)
{
    size_t iOutLens = Q_INIT_NUMBER;
    size_t iInLens = strVal.size();
    const char *pszRtn = zlibCompress(strVal.c_str(), iInLens, iOutLens);
    if (NULL == pszRtn)
    {
        return "";
    }

    return std::string(pszRtn, iOutLens);
}

const char *CEncrypt::zlibDecompression(const char *pszVal, const size_t &iValLens, size_t &iOutLens)
{
    char *pBuffer = NULL;
    int iErr = Z_OK;
    int iWSize = DEF_WBITS;
    size_t iBufferLen = (0 == m_objBuffer.getTotalLens() ? iValLens : m_objBuffer.getTotalLens());
    z_stream stStream;
    iOutLens = Q_INIT_NUMBER;

    pBuffer = m_objBuffer.New(iBufferLen);
    if (NULL == pBuffer)
    {
        return NULL;
    }

    stStream.avail_in = (uInt)iValLens;
    stStream.avail_out = (uInt)iBufferLen;
    stStream.zalloc = (alloc_func)NULL;
    stStream.zfree = (free_func)Z_NULL;
    stStream.next_out = (Byte *)pBuffer;
    stStream.next_in = (Byte *)pszVal;

    iErr = inflateInit2(&stStream, iWSize);
    switch(iErr) 
    {
    case(Z_OK):
        break;

    case(Z_MEM_ERROR):
        return NULL;

    default:
        inflateEnd(&stStream);
        return NULL;
    }

    do 
    {
        iErr = inflate(&stStream, Z_FINISH);
        switch(iErr) 
        {
        case(Z_STREAM_END):
            break;

        case(Z_BUF_ERROR):
            /*
             * If there is at least 1 byte of room according to zst.avail_out
             * and we get this error, assume that it means zlib cannot
             * process the inflate call() due to an error in the data.
             */
            if (stStream.avail_out > 0) 
            {
                inflateEnd(&stStream);
                return NULL;
            }
            /* fall through */
        case(Z_OK):
            /* need more memory */
            pBuffer = m_objBuffer.reNew(iBufferLen << 1);
            if (NULL == pBuffer) 
            {
                inflateEnd(&stStream);
                return NULL;
            }
   
            stStream.next_out = (unsigned char *)pBuffer + iBufferLen;
            stStream.avail_out = (uInt)iBufferLen;
            iBufferLen = iBufferLen << 1;
            break;

        default:
            inflateEnd(&stStream);
            return NULL;
        }
    } while (Z_STREAM_END != iErr);

    iErr = inflateEnd(&stStream);
    if (Z_OK != iErr) 
    {
        return NULL;
    }

    iOutLens = stStream.total_out;

    return pBuffer;
}

std::string CEncrypt::zlibDecode(std::string strVal)
{
    size_t iOutLens = Q_INIT_NUMBER;
    size_t iInLens = strVal.size();

    const char *pszRtn = zlibDecompression(strVal.c_str(), iInLens, iOutLens);
    if (NULL == pszRtn)
    {
        return "";
    }

    return std::string(pszRtn, iOutLens);
}