#ifndef Q_TESTLOG_H_
#define Q_TESTLOG_H_

#include "TestInclude.h"

class CTestLog : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CTestLog);   

    CPPUNIT_TEST(Test_Log);
    CPPUNIT_TEST(Test_SysLog);


    CPPUNIT_TEST_SUITE_END();

public:
    CTestLog(void){};
    ~CTestLog(void){};

private:
    void Test_Log(void);
    void Test_SysLog(void);
};

#endif//Q_TESTLOG_H_
