/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 * github:https://github.com/QService/QService
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "Library.h"

CLibrary::CLibrary(const char *pszLib) : m_pHandle(NULL)
{
    Init(pszLib);
}

CLibrary::~CLibrary(void)
{
    Destroy();
}

const char *CLibrary::getError(void)
{
#ifdef Q_OS_WIN32
    return Q_Error2Str(Q_Error());
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

#ifdef Q_OS_WIN32
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

#ifdef Q_OS_WIN32
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

#ifdef Q_OS_WIN32
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
