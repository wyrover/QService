#include "TestCharset.h"

CTestCharset::CTestCharset(void)
{

}

CTestCharset::~CTestCharset(void)
{

}

void CTestCharset::TestCharset(void)
{
    CCharset objCharset;
    std::string strMsg = "가";
    std::string strCharset;
    
    strCharset = objCharset.getStrCharset(strMsg.c_str(), strMsg.size());
    Q_Printf("%s charset is %s", strMsg.c_str(), strCharset.c_str());
    CPPUNIT_ASSERT_EQUAL(std::string("windows-1252"), strCharset);

    strMsg = "this is string test가가가*∼∼&%∼∼＄";
    strCharset = objCharset.getStrCharset(strMsg.c_str(), strMsg.size());
    Q_Printf("%s charset is %s", strMsg.c_str(), strCharset.c_str());
    CPPUNIT_ASSERT_EQUAL(std::string("windows-1252"), strCharset);

    strCharset = objCharset.getCharset("../Bin/CharsetTest/big5.txt");
    Q_Printf("file big5.txt charset is %s", strCharset.c_str());
    CPPUNIT_ASSERT_EQUAL(std::string("Big5"), strCharset);

    strCharset = objCharset.getCharset("../Bin/CharsetTest/gb18030.txt");
    Q_Printf("file gb18030.txt charset is %s", strCharset.c_str());
    CPPUNIT_ASSERT_EQUAL(std::string("gb18030"), strCharset);

    strCharset = objCharset.getCharset("../Bin/CharsetTest/shift_jis.txt");
    Q_Printf("file shift_jis.txt charset is %s", strCharset.c_str());
    CPPUNIT_ASSERT_EQUAL(std::string("Shift_JIS"), strCharset);

    strCharset = objCharset.getCharset("../Bin/CharsetTest/utf8.txt");
    Q_Printf("file utf8.txt charset is %s", strCharset.c_str());
    CPPUNIT_ASSERT_EQUAL(std::string("UTF-8"), strCharset);

    strCharset = objCharset.getCharset("../Bin/CharsetTest/ANSI.txt");
    Q_Printf("file ANSI.txt charset is %s", strCharset.c_str());
    CPPUNIT_ASSERT_EQUAL(std::string(""), strCharset);

    strCharset = objCharset.getCharset("../Bin/CharsetTest/Unicode.txt");
    Q_Printf("file Unicode.txt charset is %s", strCharset.c_str());
    CPPUNIT_ASSERT_EQUAL(std::string("UTF-16"), strCharset);
}
