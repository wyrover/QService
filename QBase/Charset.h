
#ifndef Q_CHARSET_H_
#define Q_CHARSET_H_

#include "Macros.h"

/*
* 编码格式
*/
class CCharset
{
public:
    CCharset(void);
    ~CCharset(void);

    /*获取字符编码格式,返回"" 失败或为纯ascii*/
    std::string getStrCharset(const char *pszData, const size_t iSize);
    /*获取文件编码格式, 返回"" 失败或为纯ascii*/
    std::string getCharset(const char *pszFile);
    std::string getCharset(FILE *pFile);

private:
    void *m_pCharset;
};

#endif//Q_CHARSET_H_
