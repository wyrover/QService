
#include "RSAKey.h"
#include "File.h"

CRSAKey::CRSAKey(void) : m_objAESEncode(), m_objAESDecode(), m_objBase64()
{
    Q_Zero(&m_stRandom, sizeof(m_stRandom));
    Q_Zero(&m_stPublicKey, sizeof(m_stPublicKey));
    Q_Zero(&m_stPrivateKey, sizeof(m_stPrivateKey));
}

CRSAKey::~CRSAKey(void)
{

}

/*************************************************
* Function name:creatKey
* Description  :生成key
* IN           :usKeyLens --key长度
* OUT          :None
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2013/08/10
* Modification 
* ......record :first program
**************************************************/
int CRSAKey::creatKey(const unsigned short usKeyLens)
{
    if (usKeyLens < RSAKeyMinLen
        || usKeyLens > RSAKeyMaxLen)
    {
        Q_Printf("rsa key lens %d error.key lens must in[512 - 1024]", usKeyLens);

        return Q_RTN_FAILE;
    }
    
    R_RSA_PROTO_KEY stProtoKey;

    stProtoKey.bits = usKeyLens;
    stProtoKey.useFermat4 = 1;

    R_RandomCreate(&m_stRandom);

    return R_GeneratePEMKeys(&m_stPublicKey, &m_stPrivateKey, &stProtoKey, &m_stRandom);
}

R_RSA_PUBLIC_KEY *CRSAKey::getPublicKey(void)
{
    return &m_stPublicKey;
}

R_RSA_PRIVATE_KEY *CRSAKey::getPrivateKey(void)
{
    return &m_stPrivateKey;
}

R_RANDOM_STRUCT *CRSAKey::getRandom(void)
{
    return &m_stRandom;
}

/*************************************************
* Function name:setAESKey
* Description  :设置AES加密密码,主要为RSA Key加密
* IN           :pszKey --AES密码
* OUT          :None
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2013/08/10
* Modification 
* ......record :first program
**************************************************/
int CRSAKey::setAESKey(const char *pszKey)
{
    int iRtn = Q_RTN_OK;

    iRtn = m_objAESEncode.setKey(pszKey, Key256);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "set aes encode key error.");

        return iRtn;
    }

    iRtn = m_objAESDecode.setKey(pszKey, Key256);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "set aes decode key error.");

        return iRtn;
    }

    return Q_RTN_OK;
}

int CRSAKey::fileWrite(const char *pszMsg, const size_t iLens, const char *pszFile) const
{
    FILE *pFile = NULL;

    pFile = fopen(pszFile, "wb");
    if (NULL == pFile)
    {
        Q_Printf("open file %s error.", pszFile);

        return Q_RTN_FAILE;
    }

    if (iLens != fwrite(pszMsg, 1, iLens, pFile))
    {
        fclose(pFile);
        Q_Printf("write file %s error.", pszFile);

        return Q_RTN_FAILE;
    }

    fclose(pFile);

    return Q_RTN_OK;
}

int CRSAKey::Encode(const char *pszMsg, const size_t iLens, std::string &strBase64)
{
    size_t iOutLens = Q_INIT_NUMBER;

    const char *pAES = m_objAESEncode.Encode(pszMsg, iLens, iOutLens);
    if (NULL == pAES)
    {
        Q_Printf("%s", "aes encode error.");

        return Q_RTN_FAILE;
    }

    strBase64 = m_objBase64.Encode((const unsigned char*)pAES, iOutLens);

    return Q_RTN_OK;
}

/*************************************************
* Function name:saveRandom
* Description  :保存R_RANDOM_STRUCT
* IN           :pszFile --保存到的文件
* OUT          :None
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2013/08/10
* Modification 
* ......record :first program
**************************************************/
int CRSAKey::saveRandom(const char *pszFile)
{
    int iRtn = Q_RTN_OK;
    std::string strBase64;

    iRtn = Encode((const char*)(&m_stRandom), sizeof(m_stRandom), strBase64);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    return fileWrite(strBase64.c_str(), strBase64.size(), pszFile);
}

/*************************************************
* Function name:savePublicKey
* Description  :保存公钥
* IN           :pszFile --保存到的文件
* OUT          :None
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2013/08/10
* Modification 
* ......record :first program
**************************************************/
int CRSAKey::savePublicKey(const char *pszFile)
{
    int iRtn = Q_RTN_OK;
    std::string strBase64;

    iRtn = Encode((const char*)(&m_stPublicKey), sizeof(m_stPublicKey), strBase64);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    return fileWrite(strBase64.c_str(), strBase64.size(), pszFile);
}

/*************************************************
* Function name:savePrivateKey
* Description  :保存私钥
* IN           :pszFile --保存到的文件
* OUT          :None
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2013/08/10
* Modification 
* ......record :first program
**************************************************/
int CRSAKey::savePrivateKey(const char *pszFile)
{
    int iRtn = Q_RTN_OK;
    std::string strBase64;

    iRtn = Encode((const char*)(&m_stPrivateKey), sizeof(m_stPrivateKey), strBase64);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    return fileWrite(strBase64.c_str(), strBase64.size(), pszFile);
}


std::string CRSAKey::fileRead(const char *pszFile) const
{
    int iRtn = Q_RTN_OK;
    char *pBuf = NULL;
    FILE *pFile = NULL;
    std::string strMsg;
    unsigned long ulFileSize = Q_INIT_NUMBER;

    iRtn = Q_FileSize(pszFile, ulFileSize);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("get file %s size error.", pszFile);

        return "";
    }

    pBuf = new(std::nothrow) char[ulFileSize + 1];
    if (NULL == pBuf)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

        return "";
    }

    Q_Zero(pBuf, ulFileSize + 1);

    pFile = fopen(pszFile, "rb");
    if (NULL == pFile)
    {
        Q_SafeDelete_Array(pBuf);
        Q_Printf("open file %s error.", pszFile);

        return "";
    }

    if (ulFileSize != fread(pBuf, 1, ulFileSize, pFile))
    {
        Q_SafeDelete_Array(pBuf);
        fclose(pFile);
        Q_Printf("read file %s error.", pszFile);

        return "";
    }

    strMsg = std::string(pBuf);

    Q_SafeDelete_Array(pBuf);
    fclose(pFile);

    return strMsg;
}

const char *CRSAKey::Decode(const char *pszMsg, const size_t iLens, size_t &iOutLens)
{
    size_t iAESOutLens = Q_INIT_NUMBER;
    size_t iBase64OutLens = Q_INIT_NUMBER;

    iOutLens = Q_INIT_NUMBER;

    const char *pBase64Buf = m_objBase64.Decode(pszMsg, iLens, iBase64OutLens);
    if (NULL == pBase64Buf)
    {
        Q_Printf("%s", "base64 decode error.");

        return NULL;
    }

    const char *pAESBuf = m_objAESDecode.Decode(pBase64Buf, iBase64OutLens, iAESOutLens);
    if (NULL == pAESBuf)
    {
        Q_Printf("%s", "aes decode error.");

        return NULL;
    }

    iOutLens = iAESOutLens;

    return pAESBuf;
}

int CRSAKey::loadInfo(char *pInfo, const size_t iLens, const char *pszFile)
{
    std::string strFileInfo;
    size_t iOutLens = Q_INIT_NUMBER;

    strFileInfo = fileRead(pszFile);
    if (strFileInfo.empty())
    {
        return Q_RTN_FAILE;
    }

    const char *pBuf = Decode(strFileInfo.c_str(), strFileInfo.size(), iOutLens);
    if (NULL == pBuf)
    {
        return Q_RTN_FAILE;
    }

    if (iOutLens != iLens)
    {
        return Q_RTN_FAILE;
    }

    memcpy(pInfo, pBuf, iLens);

    return Q_RTN_OK;
}

int CRSAKey::loadPublicKey(const char *pszFile)
{
    return loadInfo((char*)(&m_stPublicKey), sizeof(m_stPublicKey), pszFile);
}

int CRSAKey::loadPrivateKey(const char *pszFile)
{
    return loadInfo((char*)(&m_stPrivateKey), sizeof(m_stPrivateKey), pszFile);
}

int CRSAKey::loadRandom(const char *pszFile)
{
    return loadInfo((char*)(&m_stRandom), sizeof(m_stRandom), pszFile);
}
