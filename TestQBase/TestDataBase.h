#ifndef Q_TESTDATABASE_H_
#define Q_TESTDATABASE_H_

#include "TestInclude.h"

class CTestDataBase : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CTestDataBase);   

    CPPUNIT_TEST(Test_MySql);
    //CPPUNIT_TEST(Test_Sqlite);    

    CPPUNIT_TEST_SUITE_END();

public:
    CTestDataBase(void);
    ~CTestDataBase(void);

private:
    void Test_MySql(void);
    void Test_Sqlite(void);    

private:
    int TestDB(CDBPool *pPool);

private:
    bool m_bInitOK;
    char *m_pFileBuf;
    size_t m_iFileLens;    
    std::string m_strPath;
};

#endif//Q_TESTDATABASE_H_
