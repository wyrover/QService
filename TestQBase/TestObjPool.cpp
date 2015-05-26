
#include "TestObjPool.h"

void CTestObjPool::Test_ObjPool(void)
{
    CObjPool<STTestObj> objPool(0);
    STTestObj *pTmp = objPool.Malloc();

    CPPUNIT_ASSERT_EQUAL(0 , (int)objPool.getFreeSize());
    CPPUNIT_ASSERT_EQUAL(1 , (int)objPool.getUsedSize());

    objPool.Free(pTmp);

    CPPUNIT_ASSERT_EQUAL(1 , (int)objPool.getFreeSize());
    CPPUNIT_ASSERT_EQUAL(0 , (int)objPool.getUsedSize());

    CObjPool<STTestObj> objPool2(2);
    STTestObj *pTmp1 = objPool2.Malloc();
    STTestObj *pTmp2 = objPool2.Malloc();
    STTestObj *pTmp3 = objPool2.Malloc();
    STTestObj *pTmp4 = objPool2.Malloc();

    CPPUNIT_ASSERT_EQUAL(0 , (int)objPool2.getFreeSize());
    CPPUNIT_ASSERT_EQUAL(4 , (int)objPool2.getUsedSize());

    objPool2.Free(pTmp4);

    CPPUNIT_ASSERT_EQUAL(1 , (int)objPool2.getFreeSize());
    CPPUNIT_ASSERT_EQUAL(3 , (int)objPool2.getUsedSize());

    objPool2.Free(pTmp1);
    objPool2.Free(pTmp2);
    objPool2.Free(pTmp3);
    objPool2.Free(NULL);

    CPPUNIT_ASSERT_EQUAL(4 , (int)objPool2.getFreeSize());
    CPPUNIT_ASSERT_EQUAL(0 , (int)objPool2.getUsedSize());
}
