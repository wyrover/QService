#define _CRT_SECURE_NO_DEPRECATE
#include "TestDataBase.h"

#define MYSQL_HOST "127.0.0.1"
#define MYSQL_PORT 3306
#define MYSQL_USER "root"
#define MYSQL_PSW  "123456"
#define MYSQL_DB   "dbtest"
#define SQLIT_DB   "test.db"
#define BLOBFILE   "vld.lib"

CTestDataBase::CTestDataBase(void) : m_bInitOK(false)
{
    int iRtn = Q_RTN_OK;
    unsigned long ulFileSize = Q_INIT_NUMBER;   

    iRtn = Q_GetProPath(m_strPath);
    if (Q_RTN_OK != iRtn)
    {
        return;
    }

    iRtn = Q_FileSize((m_strPath + BLOBFILE).c_str(), ulFileSize);
    if (Q_RTN_OK != iRtn)
    {
        return;
    }

    m_iFileLens = (size_t)ulFileSize;
    m_pFileBuf = new(std::nothrow) char[m_iFileLens + 1];
    if (NULL == m_pFileBuf)
    {
        return;
    }
    Q_Zero(m_pFileBuf, m_iFileLens + 1);
    FILE *pFile = NULL;
    pFile = fopen((m_strPath + BLOBFILE).c_str(), "rb");
    if (NULL == pFile)
    {
        return;
    }

    fread(m_pFileBuf, 1, m_iFileLens, pFile);
    fclose(pFile);

    m_bInitOK = true;
}

CTestDataBase::~CTestDataBase(void)
{
    Q_SafeDelete(m_pFileBuf);
}

int CTestDataBase::TestDB(CDBPool *pPool)
{
    if (!m_bInitOK)
    {
        Q_Printf("%s", "init db pool error.");

        return Q_RTN_FAILE;
    }

    CDBStatement *pStatement = NULL;
    CDBQuery *pQuery = NULL;
    CSqlBuffer objBuf;
    CDBLink *pLink = NULL;
    std::list<int64_t> lstInt64;
    CSnowflakeID objID;
    int64_t i64 = 0;
    objID.setMachineID(123);    

    pLink = pPool->mallocLinker();
    if (NULL == pLink)
    {
        return Q_RTN_FAILE;
    }

    try
    {
        pLink->Transaction();
        pLink->execDML("delete from test");
        pLink->commitTransaction();

        objBuf.Format("insert into test(vchar1, intll, float1, smallint1, double1, blob1, int64) values (?,?,?,?,?,?,?);");
        pStatement = pLink->compileStatement(objBuf);
        pLink->Transaction();
        for (int i = 0; i < 10; i++)
        {
            pStatement->bindString(0, ("this is vchar --- " + Q_ToString(i)).c_str());
            pStatement->bindInt(1, i);
            pStatement->bindFloat(2, i + 0.2);
            pStatement->bindInt(3, i + 3);
            pStatement->bindFloat(4, i + 0.4);
            pStatement->bindBlob(5, (const unsigned char*)m_pFileBuf, m_iFileLens);
            i64 = objID.Generate();
            lstInt64.push_back(i64);
            pStatement->bindInt64(6, i64);
            pStatement->execDML();
        }
        pLink->commitTransaction();

        pStatement->finalize();

        objBuf.Format("select blob1,intll from test where intll >= ?;");
        pStatement = pLink->compileStatement(objBuf);

        std::string strInsertSql = "insert into test(vchar1, intll, float1, smallint1, double1, blob1, int64) values ('1111',1111,1111,11,1111,'',1111);";
        std::string strDelSql = "delete from test where int64=1111";
        CDBStatement *pStatement2 = pLink->compileStatement(strInsertSql.c_str());
        CDBStatement *pStatement3 = pLink->compileStatement(strDelSql.c_str());        

        for (int iii = 0; iii < 100; iii++)
        {
            pStatement->bindInt(0, 3);
            pQuery = pStatement->execQuery();
            while(!pQuery->eof())
            {
                pStatement2->execDML();
                pStatement3->execDML();
            }
            pQuery->finalize();  


            objBuf.Format("select * from test where intll >= %d;", 3);
            pQuery = pLink->execQuery(objBuf);
            while(!pQuery->eof())
            {
                CDBQuery *pQuery2 =  pLink->execQuery(objBuf);
                while(!pQuery2->eof())
                {
                    pQuery2->nextRow();
                }

                pQuery2->finalize();
                pQuery->nextRow();

                pLink->execDML(strInsertSql.c_str());
                pLink->execDML(strDelSql.c_str());               
            }

            pQuery->finalize();
        }

        pStatement->finalize();
        pStatement2->finalize();
        pStatement3->finalize();

        pLink->Transaction();
        pLink->execDML("delete from test");
        pLink->commitTransaction();

        pLink->execDML("delete from test");

        pPool->freeLinker(pLink);
    }
    catch (CException &e)
    {
        pLink->rollBack();
        pPool->freeLinker(pLink);
        Q_Printf("%d   %s", e.getErrorCode(), e.getErrorMsg());

        return Q_RTN_FAILE;
    }    

    return Q_RTN_OK;
}

void CTestDataBase::Test_MySql(void)
{
    int iRtn = Q_RTN_OK; 
    int iMaxPoolSize = 2;
    int iMinPoolSize = 1;
    CDBUrl objDBUrl;
    CDBPool MySqlPool;

    objDBUrl.Format("DB=%s;UserName=%s;PassWord=%s;Port=%d;Host=%s;",
        MYSQL_DB, MYSQL_USER, MYSQL_PSW, MYSQL_PORT, MYSQL_HOST);

    MySqlPool.setMaxFreeTime(10000);
    MySqlPool.setMaxSize(iMaxPoolSize);
    MySqlPool.setMinSize(iMinPoolSize);
    MySqlPool.setType(DBType_MySql);
    MySqlPool.setUrl(objDBUrl);

    iRtn = MySqlPool.Start();
    if (Q_RTN_OK != iRtn)
    {
        CPPUNIT_ASSERT(false);
    }

    iRtn = TestDB(&MySqlPool);
    CPPUNIT_ASSERT_EQUAL(Q_RTN_OK , iRtn);
}

void CTestDataBase::Test_Sqlite(void)
{
    int iRtn = Q_RTN_OK; 
    int iMaxPoolSize = 2;
    int iMinPoolSize = 1;
    CDBUrl objDBUrl;
    CDBPool Sqlit3Pool;

    objDBUrl.Format("DB=%s;UserName=;PassWord=;Port=;Host=;",
        (m_strPath + SQLIT_DB).c_str());

    Sqlit3Pool.setMaxFreeTime(10000);
    Sqlit3Pool.setMaxSize(iMaxPoolSize);
    Sqlit3Pool.setMinSize(iMinPoolSize);
    Sqlit3Pool.setType(DBType_Sqlite3);
    Sqlit3Pool.setUrl(objDBUrl);

    iRtn = Sqlit3Pool.Start();
    if (Q_RTN_OK != iRtn)
    {
        CPPUNIT_ASSERT(false);
    }

    iRtn = TestDB(&Sqlit3Pool);
    CPPUNIT_ASSERT_EQUAL(Q_RTN_OK , iRtn);
}
