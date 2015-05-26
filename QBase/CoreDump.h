
#ifndef Q_COREDUMP_H_
#define Q_COREDUMP_H_

#include "Macros.h"

#ifdef Q_OS_WIN
//dump¿‡
class CCoreDump
{
public:    
    explicit CCoreDump(bool bPromptUserForMiniDump);
    ~CCoreDump(void);

private:
    static LONG WINAPI unhandledExceptionHandler(struct _EXCEPTION_POINTERS *pExceptionInfo);
    void setMiniDumpFileName(void);
    bool getImpersonationToken(HANDLE* phToken) const;
    BOOL enablePrivilege(LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld) const;
    BOOL restorePrivilege(HANDLE hToken, TOKEN_PRIVILEGES* ptpOld) const;
    LONG writeMiniDump(_EXCEPTION_POINTERS *pExceptionInfo );

    _EXCEPTION_POINTERS *m_pExceptionInfo;
    TCHAR m_szMiniDumpPath[Q_FILEPATH_LENS];
    TCHAR m_szAppPath[Q_FILEPATH_LENS];
    TCHAR m_szAppBaseName[Q_FILEPATH_LENS];
    bool m_bPromptUserForMiniDump;

    static CCoreDump* s_pMiniDumper;
    static LPCRITICAL_SECTION s_pCriticalSection;

private:
    CCoreDump(void);
};
#endif

#endif//Q_COREDUMP_H_
