#ifndef Q_TESTSNOWFLAKEID_H_
#define Q_TESTSNOWFLAKEID_H_

#include "TestInclude.h"

class CTestSnowflakeID : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CTestSnowflakeID);   

    CPPUNIT_TEST(Test_SnowflakeID);

    CPPUNIT_TEST_SUITE_END();

public:
    CTestSnowflakeID(void){};
    ~CTestSnowflakeID(void){};

private:
    void Test_SnowflakeID(void);
};

#endif//Q_TESTSNOWFLAKEID_H_
