#ifndef Q_TESTENCYPT_H_
#define Q_TESTENCYPT_H_

#include "TestInclude.h"

class CTestEncypt : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CTestEncypt);   

    CPPUNIT_TEST(Test_ZLIB);
    CPPUNIT_TEST(Test_MD5);
    CPPUNIT_TEST(Test_Base64);
    CPPUNIT_TEST(Test_AES);
    CPPUNIT_TEST(Test_RSA);

    CPPUNIT_TEST_SUITE_END();

public:
    CTestEncypt(void);
    ~CTestEncypt(void);

private:
    void Test_MD5(void);
    void Test_Base64(void);
    void Test_AES(void);
    void Test_RSA(void);
    void Test_ZLIB(void);
};

#endif//Q_TESTENCYPT_H_
