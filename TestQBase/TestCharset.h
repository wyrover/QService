#ifndef Q_TESTCHARSET_H_
#define Q_TESTCHARSET_H_

#include "TestInclude.h"

class CTestCharset : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CTestCharset);   

    CPPUNIT_TEST(TestCharset);

    CPPUNIT_TEST_SUITE_END();

public:
    CTestCharset(void);
    ~CTestCharset(void);

private:
    void TestCharset(void);
}; 

#endif//Q_TESTCHARSET_H_
