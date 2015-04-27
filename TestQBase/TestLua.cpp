#include "TestLua.h"

void CTestLua::Test_Lua_Tinker(void)
{
    m_pLua = luaL_newstate();

    luaL_openlibs(m_pLua);
    luabridge::getGlobalNamespace(m_pLua)
        .beginClass<CLuaBinary>("CLuaBinary")
        .addFunction("getLens", &CLuaBinary::getLens)

        .addFunction("reSetWrite", &CLuaBinary::reSetWrite)
        .addFunction("skipRead", &CLuaBinary::skipRead)
        .addFunction("skipWrite", &CLuaBinary::skipWrite)

        .addFunction("setSint8", &CLuaBinary::setSint8)
        .addFunction("getSint8", &CLuaBinary::getSint8)
        .addFunction("setUint8", &CLuaBinary::setUint8)
        .addFunction("getUint8", &CLuaBinary::getUint8)

        .addFunction("setBool", &CLuaBinary::setBool)
        .addFunction("getBool", &CLuaBinary::getBool)

        .addFunction("setSint16", &CLuaBinary::setSint16)
        .addFunction("getSint16", &CLuaBinary::getSint16)
        .addFunction("setUint16", &CLuaBinary::setUint16)
        .addFunction("getUint16", &CLuaBinary::getUint16)

        .addFunction("setSint32", &CLuaBinary::setSint32)
        .addFunction("getSint32", &CLuaBinary::getSint32)
        .addFunction("setUint32", &CLuaBinary::setUint32)
        .addFunction("getUint32", &CLuaBinary::getUint32)

        .addFunction("setSint64", &CLuaBinary::setSint64)
        .addFunction("getSint64", &CLuaBinary::getSint64)
        .addFunction("setUint64", &CLuaBinary::setUint64)
        .addFunction("getUint64", &CLuaBinary::getUint64)

        .addFunction("setDouble", &CLuaBinary::setDouble)
        .addFunction("getDouble", &CLuaBinary::getDouble)

        .addFunction("setFloat", &CLuaBinary::setFloat)
        .addFunction("getFloat", &CLuaBinary::getFloat)

        .addFunction("setString", &CLuaBinary::setString)
        .addFunction("getString", &CLuaBinary::getString)

        .addFunction("setByte", &CLuaBinary::setByte)
        .addFunction("getByte", &CLuaBinary::getByte)

        .addFunction("setStruct", &CLuaBinary::setStruct)
        .addFunction("getStruct", &CLuaBinary::getStruct)

        .addFunction("getBuffer", &CLuaBinary::getBuffer)
        .endClass();

    std::string strPath;
    Q_GetProPath(strPath);
    strPath = Q_FormatStr("%s%s", strPath.c_str(), "LuaTest.lua");

    int iRtn = luaL_dofile(m_pLua, strPath.c_str());
    if (Q_RTN_OK != iRtn)
    {
        const char *pError = lua_tostring(m_pLua, -1);
        std::string strLuaError = ((NULL == pError) ? "" : pError);

        Q_Printf("%s", strLuaError.c_str());
        lua_close(m_pLua);
        m_pLua = NULL;

        CPPUNIT_ASSERT_EQUAL(Q_RTN_OK , iRtn);
    }

    m_objBinary.setLua(m_pLua);
    RunLua();

    if (NULL != m_pLua)
    {
        lua_close(m_pLua);
        m_pLua = NULL;
    }
}

struct Test1
{
    char cVal;
    short sVal;
    int iVal;
    bool bVal;
    float fVal;
    double dVal;
    char acVal[100];
    int iVal2;
    Test1(void)
    {
        Q_Zero(acVal, sizeof(acVal));
    };
};


struct Test2
{
    int iVal;
    Test1 stTest1;
};

struct Test3
{
    int iVal;
    Test2 stTest2;
};

void CTestLua::RunLua(void)
{
    try
    {
        Test1 stTest1;
        stTest1.cVal = 1;
        stTest1.sVal = 2;
        stTest1.iVal = 3;
        stTest1.bVal = true;
        stTest1.fVal = 4.1;
        stTest1.dVal = 5.1;
        stTest1.iVal2 = 6;
        strcpy(stTest1.acVal, "this is test!");

        Test1 *pTest1 = &stTest1;
        std::string str;

        luabridge::getGlobal(m_pLua, "TestStruct1")(&m_objBinary);
        str = m_objBinary.getBuffer();
        pTest1 = (Test1 *)str.c_str();
        m_objBinary.setBuffer(str.c_str(), str.size());
        luabridge::getGlobal(m_pLua, "TestStructRead1")(&m_objBinary);

        Test3 stTest3;
        Q_Zero(&stTest3, sizeof(stTest3));
        stTest3.iVal = 30;
        stTest3.stTest2.iVal = 20;
        stTest3.stTest2.stTest1.cVal = 1;
        stTest3.stTest2.stTest1.sVal = 2;
        stTest3.stTest2.stTest1.iVal = 3;
        stTest3.stTest2.stTest1.bVal = true;
        stTest3.stTest2.stTest1.fVal = 4.1;
        stTest3.stTest2.stTest1.dVal = 5.1;
        stTest3.stTest2.stTest1.iVal2 = 6;
        strcpy(stTest3.stTest2.stTest1.acVal, "this is test!");

        Test3 *pTest3 = &stTest3;

        luabridge::getGlobal(m_pLua, "TestStruct2")(&m_objBinary);
        str = m_objBinary.getBuffer();
        pTest3 = (Test3 *)str.c_str();
        m_objBinary.setBuffer(str.c_str(), str.size());
        luabridge::getGlobal(m_pLua, "TestStructRead2")(&m_objBinary);
    }
    catch(luabridge::LuaException &e)
    {
        Q_Printf("%s", e.what());

        return;
    }
}