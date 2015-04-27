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
    void RunLua(void);

private:
    struct lua_State *m_pLua;
    CLuaBinary m_objBinary;
};

#endif//Q_TESTLUA_H_
