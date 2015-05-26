#define _CRT_SECURE_NO_DEPRECATE

#include "TestDataBase.h"
#include "TestEncypt.h"
#include "TestFile.h"
#include "TestLog.h"
#include "TestLua.h"
#include "TestArithmetic.h"
#include "TestCharset.h"
#include "TestSnowflakeID.h"
#include "TestBinary.h"
#include "TestObjPool.h"

#ifdef Q_OS_WIN
CCoreDump objDump(true);
#include "../vld/vld.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib")
#pragma comment(lib, "libevent.lib")
#pragma comment(lib, "libevent_core.lib")
#pragma comment(lib, "libevent_extras.lib")
#if _DEBUG
#pragma comment(lib, "libcurl_a_debug.lib")
#pragma comment(lib, "cppunitd.lib")
#else
#pragma comment(lib, "libcurl_a.lib")
#pragma comment(lib, "cppunit.lib")
#endif
#pragma comment(lib, "QBase.lib")
#endif //WIN32

CPPUNIT_TEST_SUITE_REGISTRATION(CTestObjPool);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestBinary);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestLua);
//CPPUNIT_TEST_SUITE_REGISTRATION(CTestDataBase);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestArithmetic);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestEncypt);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestFile);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestLog);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestCharset);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestSnowflakeID);

bool Q_CheckBuff(const char *pSrc, const char *pDes, const size_t ilen)
{
    for (size_t i = 0; i < ilen; i++)
    {
        if (pSrc[i] != pDes[i])
        {
            return false;
        }
    }

    return true;
}

int main( int argc, char* argv[] )
{
    std::ofstream oTestRst("TestQBase.xml");
    CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();
    CPPUNIT_NS::TextUi::TestRunner runner;

    runner.addTest(suite);
    runner.setOutputter(new CPPUNIT_NS::XmlOutputter(&runner.result(), oTestRst));
    runner.run();

    return 0;
}
