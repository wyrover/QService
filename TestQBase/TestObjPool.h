
#ifndef Q_TESTOBJPOOL_H_
#define Q_TESTOBJPOOL_H_

#include "TestInclude.h"

class CTestObjPool : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CTestObjPool);   

    CPPUNIT_TEST(Test_ObjPool);

    CPPUNIT_TEST_SUITE_END();

public:
    CTestObjPool(void){};
    ~CTestObjPool(void){};

private:
    struct STTestObj
    {
        int i;
        char acTmp[32];
    };

private:
    void Test_ObjPool(void);
};

#endif//Q_TESTOBJPOOL_H_
