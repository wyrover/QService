#ifndef TEST_BINARY_H_
#define TEST_BINARY_H_

#include "TestInclude.h"

class CTestBinary : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CTestBinary);   

    CPPUNIT_TEST(Test_Binary);

    CPPUNIT_TEST_SUITE_END();

public:
    CTestBinary(void){};
    ~CTestBinary(void){};

private:
    void Test_Binary(void);
};

#endif//TEST_BINARY_H_
