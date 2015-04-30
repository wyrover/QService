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

#include "CoreDump.h"

#ifdef Q_OS_WIN32
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <strsafe.h>
#include <dbghelp.h>
#endif

#ifdef Q_OS_WIN32

#pragma warning(disable:4995)

#define DBGHELP "dbghelp.dll"
CCoreDump* CCoreDump::s_pMiniDumper = NULL;
LPCRITICAL_SECTION CCoreDump::s_pCriticalSection = NULL;

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess,
    DWORD dwPid,
    HANDLE hFile,
    MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

CCoreDump::CCoreDump(bool bPromptUserForMiniDump)
{
    assert(!s_pMiniDumper);

    s_pMiniDumper = this;
    m_bPromptUserForMiniDump = bPromptUserForMiniDump;

    s_pCriticalSection = new(std::nothrow)CRITICAL_SECTION;
    if(NULL != s_pCriticalSection)
    {
        InitializeCriticalSection(s_pCriticalSection);
    }

    ::SetUnhandledExceptionFilter(unhandledExceptionHandler);

    return;
}

CCoreDump::~CCoreDump(void)
{
    if(s_pCriticalSection)
    {
        DeleteCriticalSection(s_pCriticalSection);
        Q_SafeDelete(s_pCriticalSection);
    }

    m_pExceptionInfo = NULL;

    return;
}

LONG CCoreDump::unhandledExceptionHandler(_EXCEPTION_POINTERS *pExceptionInfo)
{
    if(NULL == s_pMiniDumper)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    return s_pMiniDumper->writeMiniDump(pExceptionInfo);
}

void CCoreDump::setMiniDumpFileName(void)
{
    time_t currentTime;
    currentTime = time(NULL);

    Q_Snprintf(m_szMiniDumpPath, sizeof(m_szMiniDumpPath) - 1, "%s%s.%ld.dmp",
        m_szAppPath, m_szAppBaseName, currentTime);

    return;
}

bool CCoreDump::getImpersonationToken(HANDLE* phToken) const
{
    *phToken = NULL;

    if(!OpenThreadToken( GetCurrentThread(),
        TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
        TRUE,
        phToken))
    {
        if(ERROR_NO_TOKEN == GetLastError())
        {
            if(!OpenProcessToken( GetCurrentProcess(),
                TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
                phToken))
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

BOOL CCoreDump::enablePrivilege(LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld) const
{
    BOOL bOk = false;

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    bOk = LookupPrivilegeValue(0, pszPriv, &tp.Privileges[0].Luid);

    if(bOk)
    {
        DWORD cbOld = sizeof(*ptpOld);
        bOk = AdjustTokenPrivileges( hToken, false, &tp, cbOld, ptpOld, &cbOld );
    }

    return (bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()));
}

BOOL CCoreDump::restorePrivilege(HANDLE hToken, TOKEN_PRIVILEGES* ptpOld) const
{
    BOOL bOk = AdjustTokenPrivileges(hToken, false, ptpOld, 0, NULL, NULL);

    return (bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()));
}


LONG CCoreDump::writeMiniDump( _EXCEPTION_POINTERS *pExceptionInfo )
{
    bool bRetval = false;
    m_pExceptionInfo = pExceptionInfo;
    HANDLE hImpersonationToken = NULL;

    if(!getImpersonationToken( &hImpersonationToken ))
    {
        return false;
    }

    HMODULE hDll = NULL;
    TCHAR szDbgHelpPath[Q_FILEPATH_LENS];

    if(GetModuleFileName( NULL, m_szAppPath, Q_FILEPATH_LENS))
    {
        TCHAR *pSlash = strchr(m_szAppPath, Q_FILEPATH_LENS);
        if(pSlash)
        {
            _tcscpy( m_szAppBaseName, pSlash + 1);
            *(pSlash+1) = 0;
        }

        strcpy(szDbgHelpPath, m_szAppPath);
        strcat(szDbgHelpPath, DBGHELP);

        hDll = ::LoadLibrary(szDbgHelpPath);
    }

    if(NULL == hDll)
    {
        hDll = ::LoadLibrary(DBGHELP);
    }

    if(NULL != hDll)
    {
        MINIDUMPWRITEDUMP MiniDumpWriteDump = 
            (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );

        if(NULL != MiniDumpWriteDump)
        {
            setMiniDumpFileName();

            HANDLE hFile = ::CreateFile(m_szMiniDumpPath, 
                GENERIC_WRITE, 
                FILE_SHARE_WRITE, 
                NULL, 
                CREATE_ALWAYS, 
                FILE_ATTRIBUTE_NORMAL, 
                NULL);

            if(INVALID_HANDLE_VALUE != hFile)
            {
                BOOL bOk;
                TOKEN_PRIVILEGES tp;                
                _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

                ExInfo.ThreadId          = ::GetCurrentThreadId();
                ExInfo.ExceptionPointers = pExceptionInfo;
                ExInfo.ClientPointers    = NULL;
                
                BOOL bPrivilegeEnabled = enablePrivilege(SE_DEBUG_NAME, hImpersonationToken, &tp);

                EnterCriticalSection(s_pCriticalSection);
                {
                     bOk = MiniDumpWriteDump(GetCurrentProcess(),
                        GetCurrentProcessId(),
                        hFile,
                        MiniDumpNormal,
                        &ExInfo,
                        NULL,
                        NULL);
                }

                LeaveCriticalSection(s_pCriticalSection);

                if(bPrivilegeEnabled)
                {
                    (void)restorePrivilege(hImpersonationToken, &tp);
                }

                if(bOk)
                {
                    bRetval = true;
                }

                ::CloseHandle( hFile );
            }
        }        
    }

    TerminateProcess(GetCurrentProcess(), 0);

    return bRetval;
}

#endif
