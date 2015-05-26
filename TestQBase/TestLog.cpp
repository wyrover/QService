#include "TestLog.h"

void CTestLog::Test_Log(void)
{
    /*CThread objThread;
    std::string strLogPath;

    objThread.Execute(new CTestLogTask());
    bool bCheck = objLog.waitForStarted();
    CPPUNIT_ASSERT(bCheck);

    int iRtn = Q_GetProPath(strLogPath);
    CPPUNIT_ASSERT_EQUAL(Q_RTN_OK , iRtn);

    g_pTxtLoger = new(std::nothrow) CTxtLoger();
    CPPUNIT_ASSERT(NULL != g_pTxtLoger);

    g_pTxtLoger->setLogFile(std::string(strLogPath + "log/Log.txt").c_str());
    g_pTxtLoger->Open();

    g_TxtLogerFD = objLog.addLoger(g_pTxtLoger);

    for (int i = 0; i < 100; i++)
    {
    Q_LOG(LOGLV_INFO, "%d", i);
    }

    Q_Sleep(2000);

    objLog.Stop();*/
}

void CTestLog::Test_SysLog(void)
{
    //Q_SYSLOG(LOGLV_WARN, "%s", Q_Now().c_str());
}
