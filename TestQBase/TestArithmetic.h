#ifndef Q_TESTARITHMETIC_H_
#define Q_TESTARITHMETIC_H_

#include "TestInclude.h"

class CTestArithmetic : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CTestArithmetic);   

    CPPUNIT_TEST(Test_Filter);

    CPPUNIT_TEST_SUITE_END();

public:
    CTestArithmetic(void){};
    ~CTestArithmetic(void){};

private:
    void Test_Filter(void);
};

#endif//Q_TESTARITHMETIC_H_
