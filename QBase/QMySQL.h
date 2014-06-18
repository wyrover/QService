/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#ifndef Q_MYSQL_H_
#define Q_MYSQL_H_

#include "DataBase.h"

#ifdef Q_WITHMYSQL
#include "mysql/mysql.h"

struct BindInfo
{
    int iIndex;
    std::string strFieldName;
};

struct BindMemInfo
{
    size_t iLens;
    char *pMem;
    BindMemInfo(void) : iLens(Q_INIT_NUMBER),  pMem(NULL)
    {

    };
};

class CMySQLQuery : public CDBQuery
{
public:
    CMySQLQuery(void);
    CMySQLQuery(MYSQL_RES *mysql_res, MYSQL *pDB);
    ~CMySQLQuery(void);
    
    int getIntField(const char* pszField, const int iNullValue = 0);  
    int64_t getInt64Field(const char* pszField, const int64_t iNullValue = 0);
    double getFloatField(const char* pszField, const double fNullValue = 0.0);
    const char* getStringField(const char* pszField, const char* pszNullValue = "");    
    const unsigned char* getBlobField(const char* pszField, int& iLen);

    bool eof(void);
    void nextRow(void);
    //释放资源
    void finalize(void);

private:
    void freeRes(void);
    void checkVM(void);
    int fieldIndex(const char* pszField);
    void setMysqlRst(MYSQL_RES *mysql_res);
    int getIntField(const int &iField, const int &iNullValue);
    int64_t getInt64Field(const int &iField, const int64_t &iNullValue);
    double getFloatField(const int &iField, const double &fNullValue);
    const char* getStringField(const int &iField, const char* pszNullValue);
    const unsigned char* getBlobField(const int &iField, int &iLen);

private:
    MYSQL_RES*		m_Mysql_Res;
    MYSQL_FIELD*	m_Field;
    MYSQL_ROW		m_Row;
    MYSQL *m_pDB;
    unsigned long	m_Row_Count;
    unsigned int	m_Field_Count;
};

/**********************************************************************/
class CMySQLStmtQuery : public CDBQuery
{
public:
    CMySQLStmtQuery(void);
    CMySQLStmtQuery(MYSQL_BIND *pRstBinder, BindInfo *pStBindInfo, 
        MYSQL_STMT *pStmt, unsigned int uiCol, MYSQL *pDB);
    ~CMySQLStmtQuery(void);
        
    int getIntField(const char* pszField, const int iNullValue = 0); 
    int64_t getInt64Field(const char* pszField, const int64_t iNullValue = 0);
    double getFloatField(const char* pszField, const double fNullValue = 0.0);    
    const char* getStringField(const char* pszField, const char* pszNullValue = "");    
    const unsigned char* getBlobField(const char* pszField, int &iLen);

    bool eof(void);
    void nextRow(void){};

    //释放资源
    void finalize(void);

private:
    void CheckIndex(int iField);
    void freeRes(void);
    int fieldIndex(const char* pszField);
    int64_t getInt64Field(const int &iField, const int64_t &iNullValue);
    double getFloatField(const int &iField, const double &dNullValue);
    const char* getStringField(const int &iField, const char* pszNullValue);
    const unsigned char* getBlobField(const int &iField, int &iLen);

private:
    MYSQL_BIND *m_pRstBinder;
    BindInfo *m_pStBindInfo;
    MYSQL_STMT *m_pStmt;
    MYSQL *m_pDB;
    unsigned int m_uiCol;
};

/**********************************************************************/
class CMySQLStatement : public CDBStatement
{
public:
    CMySQLStatement(void);
    CMySQLStatement(MYSQL *pDB, MYSQL_STMT *pStmt, bool bProcedure);
    ~CMySQLStatement(void);

    int execDML(void);
    CDBQuery *execQuery(void);

    void bindString(const int iField, const char* pszValue);
    void bindInt(const int iField, const int iValue);
    void bindInt64(const int iField, const int64_t iValue);
    void bindFloat(const int iField, const double dValue);
    void bindBlob(const int iField, const unsigned char* blobValue, const size_t iLen);
    void bindNull(const int iField);
    
    void clearParam(void);
    //释放资源
    void finalize(void);

private:
    void clearExecQuery(MYSQL_BIND *pBinder, BindInfo *pStBindInfo);
    void reSet(void);
    void freeVM(void);
    void checkDB(void);
    void checkVM(void);
    void checkBinder(void);
    void CheckParam(int iParam);
    void BindParam(void);
    void setBindInfo(size_t iLens, BindMemInfo &stInfo);
    void getRstBinderInfo(MYSQL_BIND *pBinder, BindInfo *pStBindInfo);
    void getFiledInfo(void);

private:
    MYSQL *m_pDB;
    MYSQL_STMT *m_pStmt;
    MYSQL_BIND *m_pBinder_Param;
    MYSQL_RES  *m_pMysql_Res;
    MYSQL_FIELD *m_pField;
    BindMemInfo *m_pBindMemInfo;
    unsigned int m_uiParamNum;
    unsigned int m_uiCol; 
    bool m_bProcedure;
};

/**********************************************************************/
class CMySQLLink : public CDBLink
{
public:
    CMySQLLink(void);
    ~CMySQLLink(void);

    void open(CDBUrl &objDBUrl);
    void close(void);

    void Transaction(void);
    void commitTransaction(void);
    void rollBack(void);

    int execDML(const char* szSQL);
    CDBQuery* execQuery(const char* szSQL);
    CDBStatement *compileStatement(const char* pszSQL);

    void setBusyTimeout(int nMillisecs);

    int reLink(void);

private:
    void checkDB(void);

private:
    MYSQL* m_pDb_Ptr;
    int    m_iBusyTimeoutMs;
};

#endif

#endif//Q_MYSQL_H_
