
#include "OSFunc.h"

/************************************************************************
* Function name:Q_HostName
* Description  :获取本机主机名
* IN           :NONE
* OUT          :NONE
* Return       :std::string
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
std::string Q_HostName(void)
{
    int iRtn = Q_RTN_OK;

    char acHostName[512] = {0};    
    CSockInit objSockInit;

    iRtn = gethostname(acHostName, sizeof(acHostName));
    if (Q_RTN_FAILE != iRtn)
    {
        iRtn = Q_SockError();
        Q_Printf("gethostname error. error code %d, message %s", iRtn, Q_SockError2Str(iRtn));

        return "";
    }

    acHostName[sizeof(acHostName) - 1] = '\0';

    return std::string(acHostName);
}

#ifndef Q_OS_WIN32
int ExecCmdLinux(const char *acCmd, std::list<std::string> *lstRst)
{
    int iRtn = Q_RTN_OK;
    FILE * pStream = NULL;   

    if (NULL == lstRst)
    {
        iRtn = system(acCmd);
        if (Q_RTN_OK != iRtn)
        {
            return iRtn;
        }

        return Q_RTN_OK;
    }
    else
    {
        char acTempBuf[Q_ONEK] = {0};
        pStream = popen(acCmd, "r");
        if (NULL == pStream)
        {
            Q_Printf("%s", "popen error");

            return Q_RTN_FAILE;
        }

        while(!feof(pStream))
        {
            Q_Zero(acTempBuf, sizeof(acTempBuf));
            (void)fgets(acTempBuf, sizeof(acTempBuf), pStream);
            if((0 == acTempBuf[0]) || ('\n' == acTempBuf[0]))
            {
                continue;
            }

            if (NULL != lstRst)
            {
                lstRst->push_back(std::string(acTempBuf));
            }
        }

        pclose(pStream);

        return Q_RTN_OK;
    }
}
#else
int ExecCmdWindows(const char *acCmd, const unsigned int &uiTimeOut, 
    std::list<std::string> *lstRst, unsigned long *ulRST)
{
    int iRtn = Q_RTN_OK;
    SECURITY_ATTRIBUTES sa;
    HANDLE hRead,hWrite;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
    sa.lpSecurityDescriptor = NULL;          //使用系统默认的安全描述符 
    sa.bInheritHandle = TRUE;                //创建的进程继承句柄

    if (!CreatePipe(&hRead,&hWrite,&sa,0))   //创建匿名管道
    {
        Q_Printf("%s", "CreatePipe error");

        return Q_RTN_FAILE;
    }

    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInfo;
    DWORD dwCode = 0;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.hStdError = hWrite; 
    startupInfo.hStdOutput = hWrite;         //新创建进程的标准输出连在写管道一端
    startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    startupInfo.wShowWindow = SW_HIDE;

    ZeroMemory(&processInfo,sizeof(processInfo));
    if(!CreateProcess(NULL,
        TEXT(const_cast<char*>(acCmd)),
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &startupInfo,
        &processInfo))
    {
        Q_Printf("%s", "CreateProcess error");

        return Q_RTN_FAILE;
    }

    CloseHandle(hWrite);   //关闭管道句柄

    if (WAIT_TIMEOUT == WaitForSingleObject(processInfo.hProcess, (DWORD)(1000 * uiTimeOut)))
    {
        BOOL bRNT;
        HANDLE handle;
        PROCESSENTRY32 proEntry32;
        proEntry32.dwSize = sizeof(PROCESSENTRY32);
        handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (INVALID_HANDLE_VALUE == handle)
        {
            iRtn = Q_RTN_FAILE;
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
            CloseHandle(hRead);

            return iRtn;
        }

        bRNT = Process32First(handle, &proEntry32);
        while(bRNT)
        {
            if(proEntry32.th32ParentProcessID == processInfo.dwProcessId)
            {
                HANDLE pvChild;
                pvChild = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proEntry32.th32ProcessID);
                TerminateProcess(pvChild, 0);
                CloseHandle(pvChild);
            }

            bRNT = Process32Next(handle, &proEntry32);
        }

        TerminateProcess(processInfo.hProcess, 0);
        CloseHandle(handle);
        CloseHandle(hRead);

        *ulRST = 1;
    }
    else
    {
        GetExitCodeProcess(processInfo.hProcess, &dwCode);

        *ulRST = dwCode;

        if (NULL != lstRst)
        {
            char acBuffer[Q_ONEK] = {0};
            DWORD bytesRead;
            while (true) 
            {
                Q_Zero(acBuffer, sizeof(acBuffer));
                if (FALSE == ReadFile(hRead, acBuffer, sizeof(acBuffer), 
                    &bytesRead, NULL))   //读取管道
                {
                    break;
                }

                lstRst->push_back(std::string(acBuffer));
                Sleep(10);
            }
        }
        CloseHandle(hRead);
    }

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    return Q_RTN_OK;
}
#endif

/*************************************************
* Function name:Q_ExecCmd
* Description  :执行命令，脚本
* IN           :pszCmd --脚本、命令  iSec --超时(s)
* OUT          :lstRst --执行结果
* Return       :Q_RNT_OK 成功，其他失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :first program
**************************************************/
int Q_ExecCmd(const char *pszCmd, const unsigned int uimSec, 
    std::list<std::string> *lstRst)
{
    if (NULL == pszCmd)
    {
        return Q_ERROR_NULLPOINTER;
    }

    if (NULL != lstRst)
    {
        lstRst->clear();
    }

#ifdef Q_OS_WIN32
    unsigned long ulReCode = Q_INIT_NUMBER;
    int iRtn = Q_RTN_OK;

    iRtn = ExecCmdWindows(pszCmd, uimSec, lstRst, &ulReCode);
    if ((Q_RTN_OK == iRtn)
        &&(Q_RTN_OK == ulReCode))
    {
        return Q_RTN_OK;
    }

    return Q_RTN_FAILE;
#else
    return ExecCmdLinux(pszCmd, lstRst);
#endif
}

static union  
{
    char a[4];
    unsigned long ul;
}endian = {{'L', '?', '?', 'B'}}; 
#define ENDIAN ((char)endian.ul) 

uint64_t ntohl64(uint64_t host)
{
    if ('L' == ENDIAN)
    {
        uint64_t uiRet = 0;
        unsigned long ulHigh,ulLow;

        ulLow = host & 0xFFFFFFFF;
        ulHigh = (host >> 32) & 0xFFFFFFFF;

        ulLow = ntohl(ulLow); 
        ulHigh = ntohl(ulHigh);

        uiRet = ulLow;
        uiRet <<= 32;
        uiRet |= ulHigh;

        return uiRet;
    }
    else
    {
        return host;
    }
}
