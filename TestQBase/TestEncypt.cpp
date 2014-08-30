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
    std::string strFile = "../Bin/ws2_32.dll";
    std::string strBuf = "test md5";
    CMD5 objMD5;

    objMD5.update(strBuf.c_str());
    strMD5 = objMD5.toString();
    objMD5.reset();

    objMD5.update("admin");
    strMD5 = objMD5.toString();
    objMD5.reset();
    CPPUNIT_ASSERT_EQUAL(std::string("21232f297a57a5a743894a0e4a801fc3") , strMD5);

    objMD5.updatefile(strFile.c_str());
    strMD5 = objMD5.toString();
    objMD5.reset();
    CPPUNIT_ASSERT_EQUAL(std::string("e0197a89302af5f217e6bd8148eba08b") , strMD5);
}

void CTestEncypt::Test_Base64(void)
{
    stBase64 stbs;
    std::string str = "test xxxxxxxxxxx爱疯四大(*&(!";

    Q_Zero(stbs.acTmp, sizeof(stbs.acTmp));
    memcpy(stbs.acTmp, str.c_str(), str.size());
    stbs.b1 = 1;
    stbs.i1 = 2;
    stbs.i2 = 3;
    stbs.us1 = 4;

    size_t iOutLens = 0;
    CBase64 objBase64;
    std::string  strEncode = objBase64.Encode((const unsigned char*)(str.c_str()), str.size());
    char  *pDecode = (char *)objBase64.Decode(strEncode.c_str(), strEncode.size(), iOutLens);

    CPPUNIT_ASSERT_EQUAL(str.size() , iOutLens);
    CPPUNIT_ASSERT_EQUAL(str , std::string(pDecode, iOutLens));

    strEncode = objBase64.Encode((const unsigned char*)(&stbs), sizeof(stbs));
    pDecode = (char *)objBase64.Decode(strEncode.c_str(), strEncode.size(), iOutLens);

    CPPUNIT_ASSERT_EQUAL(sizeof(stbs) , iOutLens);    

    char *pszSrc = (char*)&stbs;
    bool bCheckBuf = Q_CheckBuff(pszSrc, pDecode, iOutLens);

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

    int iRtn = Q_RTN_OK;
    AESKeyType emType = Key128;
    std::string strKey = "aaaaaaaaaaaaaaaa";
    char *pEncodeRst = NULL;
    char *pDecodeRst = NULL;
    CAESEncode objEncode;
    CAESDecode objDecode;

    size_t iOutLens = 0;
    size_t iInLens = 0;

    iRtn = objEncode.setKey(strKey.c_str(), emType);
    pEncodeRst = (char *)objEncode.Encode(str.c_str(), str.size(), iOutLens);

    iRtn = objDecode.setKey(strKey.c_str(), emType);
    iInLens = iOutLens;
    pDecodeRst = (char *)objDecode.Decode((const char*)pEncodeRst, iInLens, iOutLens);

    CPPUNIT_ASSERT_EQUAL(str.size() , iOutLens);
    CPPUNIT_ASSERT_EQUAL(str , std::string(pDecodeRst, iOutLens));

    str = "111111";
    pEncodeRst = (char *)objEncode.Encode(str.c_str(), str.size(), iOutLens);
    iInLens = iOutLens;
    pDecodeRst = (char *)objDecode.Decode((const char*)pEncodeRst, iInLens, iOutLens);

    CPPUNIT_ASSERT_EQUAL(str.size() , iOutLens);
    CPPUNIT_ASSERT_EQUAL(str , std::string(pDecodeRst, iOutLens));

    str = "111111111111111";
    pEncodeRst = (char *)objEncode.Encode(str.c_str(), str.size(), iOutLens);
    iInLens = iOutLens;
    pDecodeRst = (char *)objDecode.Decode((const char*)pEncodeRst, iInLens, iOutLens);

    CPPUNIT_ASSERT_EQUAL(str.size() , iOutLens);
    CPPUNIT_ASSERT_EQUAL(str , std::string(pDecodeRst, iOutLens));

    pEncodeRst = (char *)objEncode.Encode((const char*)(&stbs), sizeof(stbs), iOutLens);
    iInLens = iOutLens;
    pDecodeRst = (char *)objDecode.Decode((const char*)pEncodeRst, iInLens, iOutLens);

    
    CPPUNIT_ASSERT_EQUAL(sizeof(stbs) , iOutLens);

    char *pTmp = (char*)&stbs;
    bool bCheckBuf = Q_CheckBuff(pTmp, pDecodeRst, iOutLens);

    CPPUNIT_ASSERT(bCheckBuf);
}

void CTestEncypt::Test_RSA(void)
{
    CRSAKey objKey1;
    CRSAKey objKey2;
    int iRtn = Q_RTN_OK;
    unsigned short usKeyLens = 0;

    srand((unsigned int)time(NULL));
    usKeyLens = 512 + rand() % 513;
    printf("ken lens %d\n", usKeyLens);

    Q_FileDel("PrivateKey.txt");
    Q_FileDel("PublicKey.txt");
    Q_FileDel("Random.txt");

    iRtn = objKey1.creatKey(usKeyLens);
    CPPUNIT_ASSERT_EQUAL(iRtn , Q_RTN_OK);
    iRtn = objKey1.setAESKey("lqf110110");
    CPPUNIT_ASSERT_EQUAL(iRtn , Q_RTN_OK);
    iRtn = objKey2.setAESKey("lqf110110");
    CPPUNIT_ASSERT_EQUAL(iRtn , Q_RTN_OK);

    iRtn = objKey1.savePrivateKey("PrivateKey.txt");
    CPPUNIT_ASSERT_EQUAL(iRtn , Q_RTN_OK);
    iRtn = objKey1.savePublicKey("PublicKey.txt");
    CPPUNIT_ASSERT_EQUAL(iRtn , Q_RTN_OK);
    iRtn = objKey1.saveRandom("Random.txt");
    CPPUNIT_ASSERT_EQUAL(iRtn , Q_RTN_OK);

    iRtn = objKey2.loadPublicKey("PublicKey.txt");
    CPPUNIT_ASSERT_EQUAL(iRtn , Q_RTN_OK);
    iRtn = objKey2.loadPrivateKey("PrivateKey.txt");
    CPPUNIT_ASSERT_EQUAL(iRtn , Q_RTN_OK);
    iRtn = objKey2.loadRandom("Random.txt");
    CPPUNIT_ASSERT_EQUAL(iRtn , Q_RTN_OK);

    bool bCheckBuf = Q_CheckBuff((const char*)objKey1.getPrivateKey(), 
        (const char*)objKey2.getPrivateKey(), sizeof(R_RSA_PRIVATE_KEY));
    CPPUNIT_ASSERT_EQUAL(bCheckBuf , true);
    bCheckBuf = Q_CheckBuff((const char*)objKey1.getPublicKey(), 
        (const char*)objKey2.getPublicKey(), sizeof(R_RSA_PUBLIC_KEY));
    CPPUNIT_ASSERT_EQUAL(bCheckBuf , true);
    bCheckBuf = Q_CheckBuff((const char*)objKey1.getRandom(), 
        (const char*)objKey2.getRandom(), sizeof(R_RANDOM_STRUCT));
    CPPUNIT_ASSERT_EQUAL(bCheckBuf , true);

    CRSA objRSAEncrypt;
    CRSA objRSADecrypt;
    size_t iEncryptOutLens = 0;
    size_t iDecryptOutLens = 0;
    char *pRtn1 = NULL;
    char *pRtn2 = NULL;
    std::string strTest = "阿士大夫 %@$#!*()?";
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

    objRSAEncrypt.setKey(&objKey2);
    objRSADecrypt.setKey(&objKey2);

    pRtn1 = (char*)objRSAEncrypt.publicKeyEncrypt(strTest.c_str(), strTest.size(), iEncryptOutLens);
    pRtn2 = (char*)objRSADecrypt.privateKeyDecrypt(pRtn1, iEncryptOutLens, iDecryptOutLens);
    CPPUNIT_ASSERT_EQUAL(strTest.size() , iDecryptOutLens);
    CPPUNIT_ASSERT_EQUAL(strTest , std::string(pRtn2, iDecryptOutLens));

    pRtn1 = (char*)objRSAEncrypt.privateKeyEncrypt(strTest.c_str(), strTest.size(), iEncryptOutLens);
    pRtn2 = (char*)objRSADecrypt.publicKeyDecrypt(pRtn1, iEncryptOutLens, iDecryptOutLens);
    CPPUNIT_ASSERT_EQUAL(strTest.size() , iDecryptOutLens);
    CPPUNIT_ASSERT_EQUAL(strTest , std::string(pRtn2, iDecryptOutLens));

    int itmp = strTest.size();
    for (int i = 0; i < (128 - itmp); i++)
    {
        strTest += Q_ToString(i);
    }

    pRtn1 = (char*)objRSAEncrypt.publicKeyEncrypt(strTest.c_str(), strTest.size(), iEncryptOutLens);
    pRtn2 = (char*)objRSADecrypt.privateKeyDecrypt(pRtn1, iEncryptOutLens, iDecryptOutLens);
    CPPUNIT_ASSERT_EQUAL(strTest.size() , iDecryptOutLens);
    CPPUNIT_ASSERT_EQUAL(strTest , std::string(pRtn2, iDecryptOutLens));

    pRtn1 = (char*)objRSAEncrypt.privateKeyEncrypt(strTest.c_str(), strTest.size(), iEncryptOutLens);
    pRtn2 = (char*)objRSADecrypt.publicKeyDecrypt(pRtn1, iEncryptOutLens, iDecryptOutLens);
    CPPUNIT_ASSERT_EQUAL(strTest.size() , iDecryptOutLens);
    CPPUNIT_ASSERT_EQUAL(strTest , std::string(pRtn2, iDecryptOutLens));

    pRtn1 = (char*)objRSAEncrypt.publicKeyEncrypt((const char*)(&stRSATest), sizeof(stRSATest), iEncryptOutLens);
    pRtn2 = (char*)objRSADecrypt.privateKeyDecrypt(pRtn1, iEncryptOutLens, iDecryptOutLens);

    CPPUNIT_ASSERT_EQUAL(sizeof(stTestRSA) , iDecryptOutLens);

    stTestRSA *pstRSATmp = (stTestRSA *)pRtn2;
    bCheckBuf = Q_CheckBuff((const char*)pstRSATmp, 
        (const char*)(&stRSATest), sizeof(stTestRSA));
    CPPUNIT_ASSERT_EQUAL(bCheckBuf , true);

    pRtn1 = (char*)objRSAEncrypt.privateKeyEncrypt((const char*)(&stRSATest), sizeof(stRSATest), iEncryptOutLens);
    pRtn2 = (char*)objRSADecrypt.publicKeyDecrypt(pRtn1, iEncryptOutLens, iDecryptOutLens);
    CPPUNIT_ASSERT_EQUAL(sizeof(stTestRSA) , iDecryptOutLens);

    pstRSATmp = NULL;
    pstRSATmp = (stTestRSA *)pRtn2;
    bCheckBuf = Q_CheckBuff((const char*)pstRSATmp, 
        (const char*)(&stRSATest), sizeof(stTestRSA));
    CPPUNIT_ASSERT_EQUAL(bCheckBuf , true);
}
