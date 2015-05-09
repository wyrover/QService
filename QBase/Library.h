
#ifndef Q_LIBRARY_H_
#define Q_LIBRARY_H_

#include "Macros.h"

/*动态库操作类*/
class CLibrary
{
public:
    explicit CLibrary(const char *pszLib);
    ~CLibrary(void);

    /*获取dll指定函数地址*/
    void *getFuncAddr(const char *pSym);
    /*返回错误信息*/
    const char *getError(void) const;

private:
    CLibrary(void);
    void Init(const char *pszLib);
    void Destroy(void);

private:
#ifdef Q_OS_HPUX
    shl_t m_pHandle;
#else
    void *m_pHandle;
#endif
};

#endif//Q_LIBRARY_H_
