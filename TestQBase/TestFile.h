#ifndef Q_TESTFILE_H_
#define Q_TESTFILE_H_

#include "TestInclude.h"

class CTestFile : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CTestFile);   

    CPPUNIT_TEST(Test_Ini);
    CPPUNIT_TEST(Test_TableFile);

    CPPUNIT_TEST_SUITE_END();

public:
    CTestFile(void){};
    ~CTestFile(void){};

private:
    void Test_Ini(void);
    void Test_TableFile(void);
};

#endif//Q_TESTFILE_H_