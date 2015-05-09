
#include "Base64.h"

CBase64::CBase64(void) : m_objBuffer()
{
}

CBase64::~CBase64(void)
{
}

/*************************************************
* Function name:Encode
* Description  :base64编码
* IN           :pszData --需要编码的内容 iLens --内容长度
* OUT          :None
* Return       :编码后的字符串
* Make By      :lqf/200309129@163.com
* Date Time    :2013/08/10
* Modification 
* ......record :first program
**************************************************/
std::string CBase64::Encode(const unsigned char *pszData, const size_t &iLens) const
{
    //编码表
    static const char EncodeTable[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string strEncode;
    unsigned char acTmp[4] = {0};

    int iLineLength = Q_INIT_NUMBER;
    for(size_t i = 0; i < (iLens / 3); i++)
    {
        acTmp[1] = *pszData++;
        acTmp[2] = *pszData++;
        acTmp[3] = *pszData++;

        strEncode += EncodeTable[acTmp[1] >> 2];
        strEncode += EncodeTable[((acTmp[1] << 4) | (acTmp[2] >> 4)) & 0x3F];
        strEncode += EncodeTable[((acTmp[2] << 2) | (acTmp[3] >> 6)) & 0x3F];
        strEncode += EncodeTable[acTmp[3] & 0x3F];

        if(iLineLength += 4, 76 == iLineLength) 
        {
            strEncode += "\r\n";
            iLineLength = 0;
        }
    }

    //对剩余数据进行编码
    int Mod = iLens % 3;
    if(1 == Mod)
    {
        acTmp[1] = *pszData++;

        strEncode += EncodeTable[(acTmp[1] & 0xFC) >> 2];
        strEncode += EncodeTable[((acTmp[1] & 0x03) << 4)];

        strEncode += "==";
    }
    else if(2 == Mod)
    {
        acTmp[1] = *pszData++;
        acTmp[2] = *pszData++;

        strEncode += EncodeTable[(acTmp[1] & 0xFC) >> 2];
        strEncode += EncodeTable[((acTmp[1] & 0x03) << 4) | ((acTmp[2] & 0xF0) >> 4)];
        strEncode += EncodeTable[((acTmp[2] & 0x0F) << 2)];

        strEncode += "=";
    }

    return strEncode;
}

/*************************************************
* Function name:Decode
* Description  :base64解码
* IN           :pszData --需要解码的内容 iLens --内容长度
* OUT          :iOutLens --解码后的长度
* Return       :解码后的内容
* Make By      :lqf/200309129@163.com
* Date Time    :2013/08/10
* Modification 
* ......record :first program
**************************************************/
const char *CBase64::Decode(const char* pszData, const size_t &iLens, size_t &iOutLens)
{
    //解码表
    static const char acDecodeTable[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        62, // '+'
        0, 0, 0,
        63, // '/'
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
        0, 0, 0, 0, 0, 0,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
    };

    int iValue = Q_INIT_NUMBER;
    size_t i = 0;    
    char cVal = 0;
    size_t iCharLens = sizeof(cVal);

    m_objBuffer.reSet();
    iOutLens = Q_INIT_NUMBER;

    while (i < iLens)
    {
        if ((*pszData != '\r')
            && (*pszData != '\n'))
        {
            iValue = acDecodeTable[(unsigned char)*pszData++] << 18;
            iValue += acDecodeTable[(unsigned char)*pszData++] << 12;
            cVal = (iValue & 0x00FF0000) >> 16;
            m_objBuffer.pushBuff(&cVal, iCharLens);
            iOutLens++;

            if ('=' != *pszData)
            {
                iValue += acDecodeTable[(unsigned char)*pszData++] << 6;
                cVal = (iValue & 0x0000FF00) >> 8;
                m_objBuffer.pushBuff(&cVal, iCharLens);
                iOutLens++;

                if ('=' != *pszData)
                {
                    iValue += acDecodeTable[(unsigned char)*pszData++];
                    cVal = iValue & 0x000000FF;
                    m_objBuffer.pushBuff(&cVal, iCharLens);
                    iOutLens++;
                }
            }

            i += 4;
        }
        else// 回车换行,跳过
        {
            pszData++;
            i++;
        }
    }

    return m_objBuffer.getBuffer();
}
