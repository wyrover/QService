#include "TestArithmetic.h"

void CTestArithmetic::Test_Filter(void)
{
    char acWord[200] = {0};
    std::string strTmp;
    CFilter objFilter;
    bool bOK = false;

    strTmp = "毛泽东";
    bOK = objFilter.addSensitiveWord(strTmp.c_str(), strTmp.size());
    CPPUNIT_ASSERT_EQUAL(true , bOK);

    strTmp = "周恩来";
    bOK = objFilter.addSensitiveWord(strTmp.c_str(), strTmp.size());
    CPPUNIT_ASSERT_EQUAL(true , bOK);

    strTmp = "FUCK";
    bOK = objFilter.addSensitiveWord(strTmp.c_str(), strTmp.size());
    CPPUNIT_ASSERT_EQUAL(true , bOK);

    strTmp = "%%";
    bOK = objFilter.addSensitiveWord(strTmp.c_str(), strTmp.size());
    CPPUNIT_ASSERT_EQUAL(true , bOK);

    memcpy(acWord, "周恩来我xxasfdaf!%%!aaaaa!FUCK!毛泽东", 
        strlen("周恩来我xxasfdaf!%%!aaaaa!FUCK!毛泽东"));
    strTmp = "******我xxasfdaf!**!aaaaa!****!******"; 

    bOK = objFilter.checkHave(acWord, strlen(acWord));
    CPPUNIT_ASSERT_EQUAL(true , bOK);
    std::string strVal = objFilter.Filter(acWord, strlen(acWord));
    CPPUNIT_ASSERT_EQUAL(strTmp , strVal);
}
