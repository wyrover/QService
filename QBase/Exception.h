
#ifndef Q_QEXCEPTION_H_
#define Q_QEXCEPTION_H_

#include "Macros.h"

/*“Ï≥£¿‡*/
class CQException
{
public:
    CQException(const int iErroCode, const char *pszFile, 
        const char *pszFunc, const int iLine, const char *pcFormat, ...);
    ~CQException(void);

    const char* getErrorMsg(void);
    int getErrorCode(void);

private:
    CQException(void);

private:
    std::string m_strMsg;
    int m_iErroCode;    
};

#define Q_EXCEPTION(iErroCode, acFormat, ...) \
    throw CQException(iErroCode, __FILE__, __FUNCTION__, __LINE__, acFormat, ##__VA_ARGS__)

#endif//Q_QEXCEPTION_H_
