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

#ifndef Q_COREDUMP_H_
#define Q_COREDUMP_H_

#include "Macros.h"

#ifdef Q_OS_WIN32
//dump¿‡
class CCoreDump
{
public:
    explicit CCoreDump(bool bPromptUserForMiniDump);
    ~CCoreDump(void);

private:
    static LONG WINAPI unhandledExceptionHandler(struct _EXCEPTION_POINTERS *pExceptionInfo);
    void setMiniDumpFileName(void);
    bool getImpersonationToken(HANDLE* phToken);
    BOOL enablePrivilege(LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld);
    BOOL restorePrivilege(HANDLE hToken, TOKEN_PRIVILEGES* ptpOld);
    LONG writeMiniDump(_EXCEPTION_POINTERS *pExceptionInfo );

    _EXCEPTION_POINTERS *m_pExceptionInfo;
    TCHAR m_szMiniDumpPath[Q_FILEPATH_LENS];
    TCHAR m_szAppPath[Q_FILEPATH_LENS];
    TCHAR m_szAppBaseName[Q_FILEPATH_LENS];
    bool m_bPromptUserForMiniDump;

    static CCoreDump* s_pMiniDumper;
    static LPCRITICAL_SECTION s_pCriticalSection;
};
#endif

#endif//Q_COREDUMP_H_
