#include "TestBinary.h"

void CTestBinary::Test_Binary(void)
{
    CLuaBinary m_objBinary;
    std::string strTest = "this is test";
    m_objBinary.setSint8(1);
    m_objBinary.setUint8(2);
    m_objBinary.setSint16(3);
    m_objBinary.skipWrite(2);
    m_objBinary.setUint16(4);
    m_objBinary.setSint32(5);
    m_objBinary.setUint32(6);
    m_objBinary.setSint64("7");
    m_objBinary.skipWrite(4);
    m_objBinary.setSint64("5996902289472770055");
    m_objBinary.setUint64("5996902341083680779");
    m_objBinary.setUint32(strTest.size());
    m_objBinary.setString(strTest.c_str());
    m_objBinary.setFloat(float(9.01));
    m_objBinary.setByte(strTest.c_str(), 100);
    m_objBinary.setDouble(10.23);

    std::string strBin = m_objBinary.getBuffer();

    m_objBinary.setBuffer(strBin.c_str(), strBin.size());
    int iVal = (int)m_objBinary.getSint8();
    CPPUNIT_ASSERT_EQUAL(1 , iVal);
    iVal = (int)m_objBinary.getUint8();
    CPPUNIT_ASSERT_EQUAL(2 , iVal);
    iVal = (int)m_objBinary.getSint16();
    CPPUNIT_ASSERT_EQUAL(3 , iVal);
    m_objBinary.skipRead(2);
    iVal = (int)m_objBinary.getUint16();
    CPPUNIT_ASSERT_EQUAL(4 , iVal);
    iVal = (int)m_objBinary.getSint32();
    CPPUNIT_ASSERT_EQUAL(5 , iVal);
    iVal = (int)m_objBinary.getUint32();
    CPPUNIT_ASSERT_EQUAL(6 , iVal);
    std::string strVal = m_objBinary.getSint64();
    CPPUNIT_ASSERT_EQUAL(std::string("7") , strVal);
    m_objBinary.skipRead(4);
    strVal = m_objBinary.getSint64();
    CPPUNIT_ASSERT_EQUAL(std::string("5996902289472770055") , strVal);
    strVal = m_objBinary.getUint64();
    CPPUNIT_ASSERT_EQUAL(std::string("5996902341083680779") , strVal);
    unsigned int iStrLens = m_objBinary.getUint32();
    strVal = m_objBinary.getString();
    CPPUNIT_ASSERT_EQUAL(strTest , strVal);
    float fVal = m_objBinary.getFloat();
    strVal = m_objBinary.getByte(100);
    double dVal = m_objBinary.getDouble();

    return;
}
