#include "TestLog.h"

CLog objLog;

class CTestLogTask : public CTask
{
public:
    void Run()
    {
        objLog.Start();
    };
};

void CTestLog::Test_Log(void)
{
    CThread objThread;
    std::string strLogPath;

    objThread.Execute(new CTestLogTask());
    bool bCheck = objLog.waitForStarted();
    CPPUNIT_ASSERT(bCheck);

    int iRtn = Q_GetProPath(strLogPath);
    CPPUNIT_ASSERT_EQUAL(Q_RTN_OK , iRtn);

    g_pSampleLoger = new(std::nothrow) CSampleLoger();
    CPPUNIT_ASSERT(NULL != g_pSampleLoger);

    g_pSampleLoger->setLogFile(std::string(strLogPath + "log/Log.txt").c_str());
    g_pSampleLoger->Open();

    g_SampleLogerFD = objLog.addLoger(g_pSampleLoger);

    for (int i = 0; i < 100; i++)
    {
        Q_LOG(LOGLV_INFO, "%d", i);
    }

    Q_Sleep(2000);

    objLog.Stop();
}

void CTestLog::Test_SysLog(void)
{
    Q_SYSLOG(LOGLV_WARN, "%s", Q_Now().c_str());
}
