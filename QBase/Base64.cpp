
#include "Base64.h"

#ifdef WIN32
#pragma warning(disable:4267)
#endif

static const std::string base64_chars = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c) 
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

CBase64::CBase64(void)
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
std::string CBase64::Encode(const unsigned char *pszData, const size_t &iLens)
{
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    size_t iTmp = iLens;

    while (iTmp--) 
    {
        char_array_3[i++] = *(pszData++);

        if (3 == i) 
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
            {
                ret += base64_chars[char_array_4[i]];
            }

            i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
        {
            char_array_3[j] = '\0';
        }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
        {
            ret += base64_chars[char_array_4[j]];
        }

        while((i++ < 3))
        {
            ret += '=';
        }

    }

    return ret;
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
std::string CBase64::Decode(std::string &strEncoded)
{
    size_t in_len = strEncoded.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && ( strEncoded[in_] != '=') && is_base64(strEncoded[in_])) 
    {
        char_array_4[i++] = strEncoded[in_]; 
        in_++;

        if (i ==4) 
        {
            for (i = 0; i <4; i++)
            {
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
            {
                ret += char_array_3[i];
            }

            i = 0;
        }
    }

    if (i) 
    {
        for (j = i; j <4; j++)
        {
            char_array_4[j] = 0;
        }

        for (j = 0; j <4; j++)
        {
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) 
        {
            ret += char_array_3[j];
        }
    }

    return ret;
}
