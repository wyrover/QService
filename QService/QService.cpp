
#include "ServiceInit.h"

/************************************************************************
* Function name:startSV
* Description  :��������
* IN           :NONE
* OUT          :NONE
* Return       :Q_RTN_OK --�ɹ�  ���� --ʧ��
* Make By      :lqf/200309129@163.com
* Date Time    :2014/03/03
* Modification 
* ......record :first program
************************************************************************/
int startSV(void)
{
    int iRtn = Q_RTN_OK;

    iRtn = Service_InitProgram();
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "init program error!");

        return iRtn;
    }

    g_objExitMutex.Lock();
    g_objExitCond.Wait(&g_objExitMutex);    
    g_objExitMutex.unLock();

    Q_LOG(LOGLV_INFO, "%s", "get signal to stop service.");

    Service_Exit();

    return Q_RTN_OK;
}

#ifdef Q_OS_WIN
#include "../vld/vld.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib")
#pragma comment(lib, "libevent.lib")
#pragma comment(lib, "libevent_core.lib")
#pragma comment(lib, "libevent_extras.lib")
#if _DEBUG
#pragma comment(lib, "libcurl_a_debug.lib")
#else
#pragma comment(lib, "libcurl_a.lib")
#endif
#pragma comment(lib, "QBase.lib")

#define Q_STOP_SERVICE_TIMEOUT       60 * 1000      //windows ����ֹͣ��ʱʱ��
#define Q_START_SERVICE_TIMEOUT      60 * 1000       //windows ����������ʱʱ��

char m_ServiceName[Q_SERVICE_NAME_LEN] = {0};
static SERVICE_STATUS_HANDLE m_hServiceStatus;
static SERVICE_STATUS m_ServiceStatus;
static HANDLE m_hStopEvent = NULL;
int m_UnhandelExceptionHappen = 0;
char m_serviceDescription[] = {QSERVICE};

CCoreDump g_objDump(true);

typedef int (*InitAppEntry)();
typedef int (*ExitAppEntry)();

static int MAIN_InitAppBasic(void);

typedef WINADVAPI BOOL (WINAPI *CSD_T)(SC_HANDLE, DWORD, LPCVOID);

//��ʼ�������б�
InitAppEntry g_InitAppEntries[] = {MAIN_InitAppBasic, Service_InitProgram, NULL};
//ֹͣ�¼�
ExitAppEntry g_ExitAppMaunalEntries[] = {Service_Exit, NULL};
//�ػ��¼�
ExitAppEntry g_ExitAppPowerOffEntries[] = {Service_Exit, NULL};

/************************************************************************
* Function name:DGUnhandleExceptionFilter
* Description  :����δ������쳣�Ĵ���
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
long DGUnhandleExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    m_UnhandelExceptionHappen = 1;

    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

    return EXCEPTION_EXECUTE_HANDLER;
}

/************************************************************************
* Function name:MAIN_InitAppBasic
* Description  :����״̬�Ļ�����ʼ��
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static int MAIN_InitAppBasic(void)
{
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentProcess(), THREAD_PRIORITY_HIGHEST);
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)DGUnhandleExceptionFilter);

    return Q_RTN_OK;
}

/************************************************************************
* Function name:MAIN_InitStatusEX
* Description  :����״̬�ͳ�ʼ������
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static void MAIN_InitStatusEX(void)
{
    Q_Zero(&m_ServiceStatus, sizeof(m_ServiceStatus));
    m_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    m_ServiceStatus.dwCheckPoint = 0;
}

/************************************************************************
* Function name:MAIN_SetStatusEX
* Description  :���÷���Ϊָ��״̬
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static void MAIN_SetStatusEX(SERVICE_STATUS *objServieStatus)
{
    SetServiceStatus(m_hServiceStatus, objServieStatus);
}

/************************************************************************
* Function name:MAIN_SetStatus
* Description  :���÷���Ϊָ��״̬
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static void MAIN_SetStatus(DWORD dwStatus)
{
    m_ServiceStatus.dwCheckPoint = 0;
    m_ServiceStatus.dwCurrentState = dwStatus;
    SetServiceStatus(m_hServiceStatus, &m_ServiceStatus);
}

/************************************************************************
* Function name:MAIN_SetPendingStatus
* Description  :���÷���ΪPending״̬
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static void MAIN_SetPendingStatus(DWORD dwStatue, DWORD dwTimeOut)
{
    m_ServiceStatus.dwCheckPoint = 0;
    m_ServiceStatus.dwWaitHint = dwTimeOut;
    m_ServiceStatus.dwCurrentState = dwStatue;
    SetServiceStatus(m_hServiceStatus, &m_ServiceStatus);
}

/************************************************************************
* Function name:Main_ExecuteEntry
* Description  :����ִ��һ�κ����б�
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static int Main_ExecuteEntry(InitAppEntry *pEntries)
{
    int iIndex = 0;
    if (pEntries)
    {
        while(NULL != pEntries[iIndex])
        {
            m_ServiceStatus.dwCheckPoint++;
            MAIN_SetStatusEX(&m_ServiceStatus);
            if (Q_RTN_OK != (pEntries[iIndex])())
            {
                return Q_RTN_FAILE;
            }

            iIndex++;
        }
    }

    return Q_RTN_OK;
}

/************************************************************************
* Function name:MAIN_InitApp
* Description  :��������ʱ��ʼ��Ӧ���߼�����
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static int MAIN_InitApp(void)
{
    return Main_ExecuteEntry(g_InitAppEntries);
}

/************************************************************************
* Function name:MAIN_ExitAppManual
* Description  :����ֹͣ����������
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static int MAIN_ExitAppManual(void)
{
    return Main_ExecuteEntry(g_ExitAppMaunalEntries);
}

/************************************************************************
* Function name:MAIN_ExitAppPowerOff
* Description  :�ػ��¼�
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static int MAIN_ExitAppPowerOff(void)
{
    return Main_ExecuteEntry(g_ExitAppPowerOffEntries);
}

static DWORD MAIN_ServicThread(LPVOID lpParam)
{
    return 0;
}

/************************************************************************
* Function name:MAIN_StopService
* Description  :����ֹͣ��Ӧ
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static void MAIN_StopService(void)
{
    g_objExitMutex.Lock();
    g_objExitCond.Signal();
    Q_LOG(LOGLV_INFO, "%s", "signal stop service.");
    g_objExitMutex.unLock();

    MAIN_SetPendingStatus(SERVICE_STOP_PENDING, Q_STOP_SERVICE_TIMEOUT);
    SetEvent(m_hStopEvent);
    (void)MAIN_ExitAppManual();
    CloseHandle(m_hStopEvent);
    MAIN_SetStatus(SERVICE_STOPPED);
}

/************************************************************************
* Function name:MAIN_ShutDown
* Description  :�ػ��¼���Ӧ
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static void MAIN_ShutDown(void)
{
    g_objExitMutex.Lock();
    g_objExitCond.Signal();
    Q_LOG(LOGLV_INFO, "%s", "signal stop service.");
    g_objExitMutex.unLock();

    MAIN_SetPendingStatus(SERVICE_STOP_PENDING, Q_STOP_SERVICE_TIMEOUT);
    SetEvent(m_hStopEvent);
    (void)MAIN_ExitAppPowerOff();
    CloseHandle(m_hStopEvent);
    MAIN_SetStatus(SERVICE_STOPPED);
}

/************************************************************************
* Function name:MAIN_ServiceHandler
* Description  :ϵͳ�¼���Ӧ����
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
DWORD WINAPI MAIN_ServiceHandler(DWORD req, DWORD dwEvent, LPVOID lpEventData, LPVOID lpContext)
{
    switch(req)
    {
    case SERVICE_CONTROL_STOP:
        MAIN_StopService();
        break;

    case SERVICE_CONTROL_SHUTDOWN:
        MAIN_ShutDown();
        break;

    default:
        break;
    }

    return Q_RTN_OK;
}

/************************************************************************
* Function name:MAIN_ServicMain
* Description  :����������
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static void WINAPI MAIN_ServicMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    MAIN_InitStatusEX();
    m_hServiceStatus = RegisterServiceCtrlHandlerExW((LPWSTR)m_ServiceName, MAIN_ServiceHandler, NULL);
    MAIN_SetPendingStatus(SERVICE_START_PENDING, Q_START_SERVICE_TIMEOUT);
    m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hStopEvent && !MAIN_InitApp())
    {
        MAIN_SetStatus(SERVICE_RUNNING);
        WaitForSingleObject(m_hStopEvent, INFINITE);
    }

    MAIN_SetStatus(SERVICE_STOPPED);
}

/************************************************************************
* Function name:MAIN_IsInstalled
* Description  :�жϷ����Ƿ��Ѿ���װ
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static BOOL MAIN_IsInstalled(LPCTSTR lpszServiceName)
{
    BOOL bRST = FALSE;

    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM)
    {
        SC_HANDLE hService = OpenService(hSCM, lpszServiceName, SERVICE_QUERY_CONFIG);
        if (hService)
        {
            bRST = TRUE;
            CloseServiceHandle(hService);
        }

        CloseServiceHandle(hSCM);
    }

    return bRST;
}

/************************************************************************
* Function name:MAIN_StartService
* Description  :��������
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static BOOL MAIN_StartService(LPCTSTR pszServiceName)
{
    SERVICE_TABLE_ENTRY st[] = 
    {
        {(LPSTR)pszServiceName, MAIN_ServicMain},
        {NULL, NULL}
    };

    return StartServiceCtrlDispatcher(st);
}

static CSD_T getChangConfFun(void)
{
    HMODULE advapi32;
    CSD_T ChangeService_Config2;

    if (!(advapi32 = GetModuleHandle("ADVAPI32.DLL")))
    {
        return NULL;
    }

    if (!(ChangeService_Config2 = (CSD_T) GetProcAddress(advapi32, "ChangeServiceConfig2A")))
    {
        return NULL;
    }

    return ChangeService_Config2;
}

/************************************************************************
* Function name:MAIN_InstallService
* Description  :��װ����
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static BOOL MAIN_InstallService(LPCTSTR lpSVNam)
{
    char szFilePath[Q_FILEPATH_LENS];
    SC_HANDLE hService;
    CSD_T ChangeService_Config2;

    ChangeService_Config2 = getChangConfFun();
    if (NULL == ChangeService_Config2)
    {
        return FALSE;
    }

    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hSCM)
    {
        return FALSE;
    }

    memset(szFilePath, 0, sizeof(szFilePath));
    GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));
    char acTmp[Q_ONEK] = {0};
    Q_Snprintf(acTmp, sizeof(acTmp) - 1, "\"%s\" \"-r\" \"%s\"", szFilePath, lpSVNam);
    hService = CreateService(hSCM,
        lpSVNam,
        lpSVNam,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,// | SERVICE_INTERACTIVE_PROCESS(������������潻��),
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        acTmp,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);

    if (!hService)
    {
        CloseServiceHandle(hSCM);
        return FALSE;
    }    

    SERVICE_DESCRIPTION sdBuf;
    sdBuf.lpDescription = m_serviceDescription;
    ChangeService_Config2(
        hService,                                // handle to service
        SERVICE_CONFIG_DESCRIPTION,             // change: description
        &sdBuf);                                // new data

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);

    return TRUE;
}

/************************************************************************
* Function name:MAIN_UnInstallService
* Description  :ж�ط���
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
************************************************************************/
static BOOL MAIN_UnInstallService(LPCTSTR lpszServiceNam)
{
    BOOL bRST = FALSE;
    SC_HANDLE hService;
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hSCM)
    {
        return FALSE;
    }

    hService = OpenService(hSCM, lpszServiceNam, DELETE);
    if (hService)
    {
        if (DeleteService(hService))
        {
            bRST = TRUE;
        }

        CloseServiceHandle(hService);
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);

    return bRST;
}

static void PrintUseAge()
{
    printf("%s\n", "UseAge:use -i \"service name\" to install service;");
    printf("%s\n", "-u \"service name\" to uninstall service;");
    printf("%s\n", "-r \"service name\" to start service;");
    printf("%s\n", "-v show server vesion;");
    printf("%s\n", "no param start server in command line.");
}

BOOL WINAPI ConsoleHandler(DWORD msgType)
{
    BOOL bRtn = FALSE;

    switch(msgType) 
    { 
        //CTRL+C
    case CTRL_C_EVENT: 
        bRtn = TRUE;
        break;

        //�رտ���̨
    case CTRL_CLOSE_EVENT: 
        bRtn = TRUE;
        break; 

        //CTRL+BREAK 
    case CTRL_BREAK_EVENT: 
        bRtn = TRUE;
        break;

        //�û��˳�
    case CTRL_LOGOFF_EVENT: 
        bRtn = TRUE;
        break;

        //ϵͳ���ر�
    case CTRL_SHUTDOWN_EVENT: 
        bRtn = TRUE;
        break;

    default: 
        bRtn = FALSE;
        break;
    } 

    if (bRtn)
    {
        g_objExitMutex.Lock();
        g_objExitCond.Signal();
        g_objExitMutex.unLock();
    }

    return bRtn;
}

int main(int argc, char *argv[])
{
    int iRtn = Q_RTN_OK;
    std::string strFullPath;
    std::string strReq;

    iRtn = Q_GetProPath(strFullPath);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "failed to get program path.");

        return iRtn;
    }

    Q_Zero(g_acModulPath, sizeof(g_acModulPath));
    memcpy(g_acModulPath, strFullPath.c_str(), strFullPath.size());

    if (1 == argc)
    {
        (void)SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);

        return startSV();
    }

    if (2 == argc)
    {
        if (std::string("-v") == std::string(argv[1]))
        {
            printf("%d.%d.%d\n", Q_MAJOR, Q_MINOR, Q_RELEASE);

            return Q_RTN_OK;
        }

        PrintUseAge();
        
        return Q_RTN_FAILE;
    }

    if (3 != argc)
    {
        PrintUseAge();

        return Q_RTN_FAILE;
    }
    
    strReq = std::string(argv[1]);

    //��װ����
    if (std::string("-i") ==  strReq)
    {
        if (!MAIN_IsInstalled(argv[2]))
        {
            if (!MAIN_InstallService(argv[2]))
            {
                Q_Printf("install service %s error!", argv[2]);

                return Q_RTN_FAILE;
            }
            else
            {
                Q_Printf("install service %s successfully!", argv[2]);
                return Q_RTN_OK;
            }
        }
        else
        {
            Q_Printf("service %s exited!", argv[2]);

            return Q_RTN_FAILE;
        }
    }
    //ж�ط���
    else if (std::string("-u") ==  strReq)
    {
        if (!MAIN_IsInstalled(argv[2]))
        {
            Q_Printf("uninstall service error.Service %s not exited!", argv[2]);

            return Q_RTN_FAILE;
        }
        else
        {
            if (MAIN_UnInstallService(argv[2]))
            {
                Q_Printf("uninstall service %s successfully!", argv[2]);

                return Q_RTN_OK;
            }
            else
            {
                Q_Printf("uninstall service %s failed!", argv[2]);

                return Q_RTN_FAILE;
            }					
        }
    }
    //���з���
    else if (std::string("-r") ==  strReq)
    {
        if (MAIN_StartService(argv[2]))
        {
            Q_Printf("start service %s successfully!", argv[2]);

            return Q_RTN_OK;
        }
        else
        {
            Q_Printf("start service %s failed!", argv[2]);

            return Q_RTN_FAILE;
        }
    }
    else
    {
        PrintUseAge();

        return Q_RTN_FAILE;
    }
}

#else

static void PrintUseAge()
{
    printf("%s\n", "UseAge:\"./QService\" run server in background;");
    printf("%s\n", "\"./QService -d\" run server in  frontground;");
    printf("%s\n", "\"./QService -v\" show server version!");
}

/*
RLIMIT_AS //���̵�������ڴ�ռ�
RLIMIT_CORE //�ں�ת���ļ�����󳤶ȡ�
RLIMIT_CPU //��������CPUʹ��ʱ��
RLIMIT_DATA //�������ݶε����ֵ��
RLIMIT_FSIZE //���̿ɽ������ļ�����󳤶ȡ����������ͼ������һ����ʱ�����Ļ���䷢��SIGXFSZ�źţ�Ĭ������½���ֹ���̵�ִ�С�
RLIMIT_LOCKS //���̿ɽ������������޵����ֵ��
RLIMIT_MEMLOCK //���̿��������ڴ��е�������������ֽ�Ϊ��λ��
RLIMIT_MSGQUEUE //���̿�ΪPOSIX��Ϣ���з��������ֽ�����
RLIMIT_NICE //���̿�ͨ��setpriority() �� nice()�������õ��������ֵ��
RLIMIT_NOFILE //ָ���Ƚ��̿ɴ򿪵�����ļ������ʴ�һ��ֵ��������ֵ���������EMFILE����
RLIMIT_NPROC //�û���ӵ�е�����������
RLIMIT_RTPRIO //���̿�ͨ��sched_setscheduler �� sched_setparam���õ����ʵʱ���ȼ���
RLIMIT_SIGPENDING //�û���ӵ�е��������ź�����
RLIMIT_STACK //���Ľ��̶�ջ�����ֽ�Ϊ��λ��
*/
int setRLimit(int iResource)
{
    struct rlimit stLimitOld;
    struct rlimit stLimNew;

    Q_Printf("setrlimit %d", iResource);
    if (getrlimit(iResource, &stLimitOld) < 0)
    {
        Q_Printf("getrlimit failed: %s, resource %d", strerror(errno), iResource);
        return Q_RTN_ERROR;
    }

    Q_Printf("get rlim_cur %d, rlim_max %d.", stLimitOld.rlim_cur, stLimitOld.rlim_max);

    stLimNew.rlim_cur = stLimitOld.rlim_max;
    stLimNew.rlim_max = stLimitOld.rlim_max;
    Q_Printf("set rlim_cur %d, rlim_max %d.", stLimNew.rlim_cur, stLimNew.rlim_max);
    if (setrlimit(iResource, &stLimNew) < 0)
    {
        Q_Printf("setrlimit failed: %s, resource %d", strerror(errno), iResource);
        (void)setrlimit(iResource, &stLimitOld);

        return Q_RTN_ERROR;
    } 

    return Q_RTN_OK;
}

int main(int argc, char *argv[])
{
    int iRtn = Q_RTN_OK;
    std::string strDir;

    signal(SIGPIPE, SIG_IGN);//��ĳһ�˹ر����ӣ�����һ����Ȼ����д���ݣ���һ��д���ݺ���յ�RST��Ӧ���˺���д���ݣ��ں˽�����̷���SIGPIPE�ź�
    signal(SIGINT, SigHandEntry);//��ֹ����
    signal(SIGHUP, SigHandEntry);//��ֹ����
    signal(SIGTSTP, SigHandEntry);//ctrl+Z
    signal(SIGTERM, SigHandEntry);//��ֹһ������
    signal(SIGKILL, SigHandEntry);//������������
    signal(SIGABRT, SigHandEntry);//��ֹһ������
    signal(Q_SIGNAL_EXIT, SigHandEntry);

    Q_Printf("exit service by command \"kill -%d + pid\".", Q_SIGNAL_EXIT);

    //��dump�ļ�����
    iRtn = setRLimit(RLIMIT_CORE);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    iRtn = Q_GetProPath(strDir);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "failed to get program path.");

        return Q_RTN_FAILE;
    }

    Q_Zero(g_acModulPath, sizeof(g_acModulPath));
    memcpy(g_acModulPath, strDir.c_str(), strDir.size());    

    if (argc > 1)
    {
        if (2 != argc)
        {
            PrintUseAge();

            return Q_RTN_FAILE;
        }

        if (std::string("-d") == std::string(argv[1]))
        {
            return startSV();
        }

        if (std::string("-v") == std::string(argv[1]))
        {
            printf("%d.%d.%d\n", Q_MAJOR, Q_MINOR, Q_RELEASE);

            return Q_RTN_OK;
        }

        PrintUseAge();

        return Q_RTN_FAILE;
    }

    pid_t iPid = fork();
    if (0 == iPid)
    {
        return startSV();
    }
    else if (iPid > 0)
    {
        return Q_RTN_OK;
    }
    else
    {
        Q_Printf("%s", "fork process error!");

        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}
#endif//Q_OS_Win32
