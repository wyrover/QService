#ifndef Q_TESTLUA_H_
#define Q_TESTLUA_H_

#include "TestInclude.h"

class CTestLua : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CTestLua);   

    CPPUNIT_TEST(Test_Lua_Tinker);

    CPPUNIT_TEST_SUITE_END();

public:
    CTestLua(void){};
    ~CTestLua(void){};

private:
    void Test_Lua_Tinker(void);
};

#endif//Q_TESTLUA_H_
