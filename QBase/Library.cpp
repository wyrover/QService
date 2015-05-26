
#include "Library.h"

CLibrary::CLibrary(const char *pszLib) : m_pHandle(NULL)
{
    Init(pszLib);
}

CLibrary::~CLibrary(void)
{
    try
    {
        Destroy();
    }
    catch(...)
    {

    }
}

const char *CLibrary::getError(void) const
{
#ifdef Q_OS_WIN
    return Q_Error2Str((int)Q_Error());
#else
    return dlerror();
#endif
}

/************************************************************************
* Function name:getFuncAddr
* Description  :获取dll指定函数地址
* IN           :pSym --函数名
* OUT          :NONE
* Return       :void *
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
void *CLibrary::getFuncAddr(const char *pSym)
{
    if (NULL == pSym)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);
        return NULL;
    }

    if (NULL == m_pHandle)
    {
        Q_Printf("%s", "dll handle is null.");
        return NULL;
    }

#ifdef Q_OS_WIN
    return GetProcAddress((HMODULE)m_pHandle, pSym);
#else
    #ifdef Q_OS_HPUX
        void *pFunc = NULL;
        shl_findsym(m_pHandle, pSym, TYPE_PROCEDURE, pFunc);

        return pFunc;
    #else
        return dlsym(m_pHandle, pSym);
    #endif
#endif
}

void CLibrary::Init(const char *pszLib)
{
    if (NULL == pszLib)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return;
    }

#ifdef Q_OS_WIN
    m_pHandle = LoadLibrary(pszLib);
#else
    #ifdef Q_OS_HPUX
        m_pHandle = shl_load(pszLib, BIND_IMMEDIATE, 0);
    #else
        m_pHandle = dlopen(pszLib, RTLD_LAZY);
    #endif
#endif
}

void CLibrary::Destroy(void)
{
    if (NULL == m_pHandle)
    {
        return;
    }

#ifdef Q_OS_WIN
    (void)FreeLibrary((HMODULE)m_pHandle);
#else
    #ifdef Q_OS_HPUX
        shl_unload(m_pHandle);
    #else
        (void)dlclose(m_pHandle);
    #endif
#endif

    m_pHandle = NULL;
}
