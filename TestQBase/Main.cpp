#define _CRT_SECURE_NO_DEPRECATE

#include "TestDataBase.h"
#include "TestEncypt.h"
#include "TestFile.h"
#include "TestLog.h"
#include "TestLua.h"
#include "TestArithmetic.h"
#include "TestCharset.h"
#include "TestSnowflakeID.h"

#ifdef Q_OS_WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "../Bin/libmysql.lib")
#pragma comment(lib, "../Bin/libevent_core.lib")
#pragma comment(lib, "../Bin/libevent_extras.lib")
#pragma comment(lib, "../Bin/libcurl_a.lib")
#pragma comment(lib, "../Bin/QBase.lib")
#ifdef _DEBUG
#pragma comment(lib, "../Bin/cppunitd.lib")
#else
#pragma comment(lib, "../Bin/cppunit.lib")
#endif // _DEBUG
#endif //WIN32

CPPUNIT_TEST_SUITE_REGISTRATION(CTestDataBase);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestArithmetic);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestEncypt);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestFile);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestLog);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestLua);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestCharset);
CPPUNIT_TEST_SUITE_REGISTRATION(CTestSnowflakeID);

CCoreDump objDump(true);

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
