#include "TestEncypt.h"

struct stBase64
{
    bool b1;
    unsigned short us1;
    int i1;
    int i2;
    char acTmp[64];
};

struct stTestRSA
{
    bool b;
    char c;
    int i;
    long l;
    char ac1[8];
    char ac2[128];
    size_t iii;
    stTestRSA()
    {
        memset(ac1, 0, sizeof(ac1));
        memset(ac2, 0, sizeof(ac2));
    };
};

CTestEncypt::CTestEncypt(void)
{

}

CTestEncypt::~CTestEncypt(void)
{

}

void CTestEncypt::Test_MD5(void)
{
    std::string strMD5;
    std::string strFile = "../Bin/dbghelp.dll";

    strMD5 = CEncrypt::getSingletonPtr()->md5Str(std::string("admin"));
    CPPUNIT_ASSERT_EQUAL(std::string("21232f297a57a5a743894a0e4a801fc3") , strMD5);

    strMD5 = CEncrypt::getSingletonPtr()->md5File(strFile);
    CPPUNIT_ASSERT_EQUAL(std::string("5c5e3afd499e5146fef1da5ef8a23205") , strMD5);
}

void CTestEncypt::Test_Base64(void)
{
    stBase64 stbs;
    std::string str = "test xxxxxxxxxxx°®·èËÄ´ó(*&(!";

    Q_Zero(stbs.acTmp, sizeof(stbs.acTmp));
    memcpy(stbs.acTmp, str.c_str(), str.size());
    stbs.b1 = 1;
    stbs.i1 = 2;
    stbs.i2 = 3;
    stbs.us1 = 4;

    std::string  strEncode = CEncrypt::getSingletonPtr()->b64Encode(str);
    std::string strDecode = CEncrypt::getSingletonPtr()->b64Decode(strEncode);

    CPPUNIT_ASSERT_EQUAL(str , strDecode);

    strEncode = CEncrypt::getSingletonPtr()->b64Encode(std::string((const char*)(&stbs), sizeof(stbs)));
    strDecode = CEncrypt::getSingletonPtr()->b64Decode(strEncode); 

    char *pszSrc = (char*)&stbs;
    bool bCheckBuf = Q_CheckBuff(pszSrc, strDecode.c_str(), strDecode.size());

    CPPUNIT_ASSERT(bCheckBuf);
}

void CTestEncypt::Test_AES(void)
{
    stBase64 stbs;
    std::string str = "11111111111111111";
    Q_Zero(stbs.acTmp, sizeof(stbs.acTmp));
    memcpy(stbs.acTmp, str.c_str(), str.size());
    stbs.b1 = 1;
    stbs.i1 = 2;
    stbs.i2 = 3;
    stbs.us1 = 4;

    AESKeyType emType = AESKEY_128;
    std::string strKey = "aaaaaaaaaaaaaaaa";
    std::string strEncodeRst;
    std::string strDecodeRst;

    CEncrypt::getSingletonPtr()->setAESKey(strKey.c_str(), emType);

    strEncodeRst = CEncrypt::getSingletonPtr()->aesEncode(str);
    strDecodeRst = CEncrypt::getSingletonPtr()->aesDecode(strEncodeRst);
    CPPUNIT_ASSERT_EQUAL(str, strDecodeRst);

    str = "111111";
    strEncodeRst = CEncrypt::getSingletonPtr()->aesEncode(str);
    strDecodeRst = CEncrypt::getSingletonPtr()->aesDecode(strEncodeRst);
    CPPUNIT_ASSERT_EQUAL(str, strDecodeRst);

    str = "111111111111111";
    strEncodeRst = CEncrypt::getSingletonPtr()->aesEncode(str);
    strDecodeRst = CEncrypt::getSingletonPtr()->aesDecode(strEncodeRst);
    CPPUNIT_ASSERT_EQUAL(str, strDecodeRst);

    strEncodeRst = CEncrypt::getSingletonPtr()->aesEncode(std::string((const char*)(&stbs), sizeof(stbs)));
    strDecodeRst = CEncrypt::getSingletonPtr()->aesDecode(strEncodeRst);

    CPPUNIT_ASSERT_EQUAL(sizeof(stbs), strDecodeRst.size());

    char *pTmp = (char*)&stbs;
    bool bCheckBuf = Q_CheckBuff(pTmp, strDecodeRst.c_str(), strDecodeRst.size());

    CPPUNIT_ASSERT(bCheckBuf);
}

void CTestEncypt::Test_RSA(void)
{
    CRSAKey objKey1;
    int iRtn = Q_RTN_OK;
    unsigned short usKeyLens = 0;

    srand((unsigned int)time(NULL));
    usKeyLens = 512 + rand() % 513;
    printf("key lens %d\n", usKeyLens);

    Q_FileDel("PrivateKey.txt");
    Q_FileDel("PublicKey.txt");
    Q_FileDel("Random.txt");

    iRtn = objKey1.creatKey(usKeyLens);
    CPPUNIT_ASSERT_EQUAL(iRtn, Q_RTN_OK);

    iRtn = objKey1.savePrivateKey("PrivateKey.txt");
    CPPUNIT_ASSERT_EQUAL(iRtn, Q_RTN_OK);
    iRtn = objKey1.savePublicKey("PublicKey.txt");
    CPPUNIT_ASSERT_EQUAL(iRtn, Q_RTN_OK);
    iRtn = objKey1.saveRandom("Random.txt");
    CPPUNIT_ASSERT_EQUAL(iRtn, Q_RTN_OK);

    iRtn = CEncrypt::getSingletonPtr()->setRSAKey("PublicKey.txt", "PrivateKey.txt", "Random.txt");
    CPPUNIT_ASSERT_EQUAL(iRtn, Q_RTN_OK);

   
    std::string strEncodeRst;
    std::string strDecodeRst;
    std::string strTest = "°¢Ê¿´ó·ò %@$#!*()?";
    stTestRSA stRSATest;

    stRSATest.b = false;
    stRSATest.c = 18;
    stRSATest.i = 19004;
    stRSATest.l = 1111111;
    stRSATest.iii = 845892;
    memcpy(stRSATest.ac1, "123456", strlen("123456"));
    for (int i = 0; i < (int)sizeof(stRSATest.ac2); i++)
    {
        stRSATest.ac2[i] = i + 1;
    }
    stRSATest.ac2[sizeof(stRSATest.ac2) - 1] = '\0';

    strEncodeRst = CEncrypt::getSingletonPtr()->rsaPubEncode(strTest);
    strDecodeRst = CEncrypt::getSingletonPtr()->rsaPriDecode(strEncodeRst);
    CPPUNIT_ASSERT_EQUAL(strTest, strDecodeRst);

    strEncodeRst = CEncrypt::getSingletonPtr()->rsaPriEncode(strTest);
    strDecodeRst = CEncrypt::getSingletonPtr()->rsaPubDecode(strEncodeRst);
    CPPUNIT_ASSERT_EQUAL(strTest, strDecodeRst);

    int itmp = strTest.size();
    for (int i = 0; i < (128 - itmp); i++)
    {
        strTest += Q_ToString(i);
    }

    strEncodeRst = CEncrypt::getSingletonPtr()->rsaPubEncode(strTest);
    strDecodeRst = CEncrypt::getSingletonPtr()->rsaPriDecode(strEncodeRst);
    CPPUNIT_ASSERT_EQUAL(strTest, strDecodeRst);

    strEncodeRst = CEncrypt::getSingletonPtr()->rsaPriEncode(strTest);
    strDecodeRst = CEncrypt::getSingletonPtr()->rsaPubDecode(strEncodeRst);
    CPPUNIT_ASSERT_EQUAL(strTest, strDecodeRst);

    strEncodeRst = CEncrypt::getSingletonPtr()->rsaPubEncode(std::string((const char*)(&stRSATest), sizeof(stRSATest)));
    strDecodeRst = CEncrypt::getSingletonPtr()->rsaPriDecode(strEncodeRst);
    CPPUNIT_ASSERT_EQUAL(sizeof(stTestRSA) , strDecodeRst.size());

    stTestRSA *pstRSATmp = (stTestRSA *)strDecodeRst.c_str();
    bool bCheckBuf = Q_CheckBuff((const char*)pstRSATmp, 
        (const char*)(&stRSATest), sizeof(stTestRSA));
    CPPUNIT_ASSERT_EQUAL(bCheckBuf , true);

    strEncodeRst = CEncrypt::getSingletonPtr()->rsaPriEncode(std::string((const char*)(&stRSATest), sizeof(stRSATest)));
    strDecodeRst = CEncrypt::getSingletonPtr()->rsaPubDecode(strEncodeRst);
    CPPUNIT_ASSERT_EQUAL(sizeof(stTestRSA) , strDecodeRst.size());

    pstRSATmp = NULL;
    pstRSATmp = (stTestRSA *)strDecodeRst.c_str();
    bCheckBuf = Q_CheckBuff((const char*)pstRSATmp, 
        (const char*)(&stRSATest), sizeof(stTestRSA));
    CPPUNIT_ASSERT_EQUAL(bCheckBuf , true);
}

void CTestEncypt::Test_ZLIB(void)
{
    std::string strRtn;
    std::string strSrc = "12334555aaaaa*&^&%²âÊÔaaaaaaavdsaaaaaaaaaaaaxvcxvxcvxcaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!²âÊÔ&";

    strRtn = CEncrypt::getSingletonPtr()->zlibEncode(strSrc);
    strRtn = CEncrypt::getSingletonPtr()->zlibDecode(strRtn);

    CPPUNIT_ASSERT_EQUAL(strRtn , strSrc);
}
