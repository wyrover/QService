
#ifndef Q_SQLITE_H_
#define Q_SQLITE_H_

#include "../DataBase.h"
#include "Sqlite/sqlite3.h"

class CSQLite3Query : public CDBQuery
{
public:
    CSQLite3Query(void);
    CSQLite3Query(sqlite3* pDB,
        sqlite3_stmt* pVM,
        bool bEof,
        bool bOwnVM=true);
    ~CSQLite3Query(void);
    
    int getIntField(const char* pszField, const int iNullValue = 0);  
    int64_t getInt64Field(const char* pszField, const int64_t iNullValue = 0);
    double getFloatField(const char* pszField, const double fNullValue = 0.0);    
    const char* getStringField(const char* pszField, const char* pszNullValue = "");    
    const unsigned char* getBlobField(const char* pszField, int &iLen);

    bool eof(void);
    void nextRow(void);
    //释放资源
    void finalize(void);

private:
    void freeRes(void);
    int fieldIndex(const char* pszField);
    int fieldDataType(int iCol);
    void checkVM(void);
    int getIntField(const int &iField, const int &iNullValue);
    int64_t getInt64Field(const int &iField, const int64_t &iNullValue);
    double getFloatField(const int &iField, const double &fNullValue);
    const char* getStringField(const int &iField, const char* pszNullValue);
    const unsigned char* getBlobField(const int &iField, int &iLen);

private:    
    bool m_bEof;    
    bool m_bOwnVM;
    int m_iCols;
    sqlite3* m_pDB;
    sqlite3_stmt* m_pVM;
};

/**********************************************************************/
class CSQLite3Statement : public CDBStatement
{
public:
    CSQLite3Statement(void);
    CSQLite3Statement(sqlite3* pDB, sqlite3_stmt* pVM);
    ~CSQLite3Statement(void);

    int execDML(void);
    CDBQuery *execQuery(void);

    void bindString(const int iField, const char* pszValue);
    void bindInt(const int iField, const int iValue);
    void bindInt64(const int iField, const int64_t iValue);
    void bindFloat(const int iField, const double dValue);
    void bindBlob(const int iField, const unsigned char* blobValue, const size_t iLen);
    void bindNull(const int iField);

    //释放资源
    void finalize(void);

private:
    void freeVM(void);
    void checkDB(void);
    void checkVM(void);
    void CheckParam(int iParam); 
    int bindParameterIndex(const char* pszParam);

private:
    sqlite3* m_pDB;
    sqlite3_stmt* m_pVM;
    int m_iCols;
};

/**********************************************************************/
class CSQLite3Link : public CDBLink
{
public:
    CSQLite3Link(void);
    ~CSQLite3Link(void);

    void open(CDBUrl &objDBUrl);
    void close(void);

    void Transaction(void);
    void commitTransaction(void);
    void rollBack(void);

    int execDML(const char* szSQL);
    CDBQuery* execQuery(const char* szSQL);
    CDBStatement *compileStatement(const char* pszSQL);

    void setBusyTimeout(int nMillisecs);

private:
    sqlite3_stmt* compile(const char* szSQL);
    void checkDB(void);

private:
    sqlite3* m_pDB;
    int m_iBusyTimeoutMs;
};

#endif//Q_SQLITE_H_
