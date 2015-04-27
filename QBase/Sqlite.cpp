/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 * github:https://github.com/QService/QService
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

#include "Sqlite.h"

CSQLite3Query::CSQLite3Query(void)
{
    m_bEof = true;
    m_bOwnVM = false;
    m_iCols = 0;
    m_pDB = NULL;
    m_pVM = NULL;
}

CSQLite3Query::CSQLite3Query(sqlite3* pDB,
    sqlite3_stmt* pVM,
    bool bEof,
    bool bOwnVM)
{
    m_pDB = pDB;
    m_pVM = pVM;
    m_bEof = bEof;
    m_iCols = sqlite3_column_count(m_pVM);
    m_bOwnVM = bOwnVM;
}

CSQLite3Query::~CSQLite3Query(void)
{
    try
    {
        freeRes();
    }
    catch (...)
    {
    }
}

int CSQLite3Query::getIntField(const int &iField, const int &iNullValue)
{
    if (SQLITE_NULL == fieldDataType(iField))
    {
        return iNullValue;
    }
    else
    {
        return sqlite3_column_int(m_pVM, iField);
    }
}

int CSQLite3Query::getIntField(const char* pszField, const int iNullValue)
{
    int iField = fieldIndex(pszField);

    return getIntField(iField, iNullValue);
}

int64_t CSQLite3Query::getInt64Field(const int &iField, const int64_t &iNullValue)
{
    if (SQLITE_NULL == fieldDataType(iField))
    {
        return iNullValue;
    }
    else
    {
        return sqlite3_column_int64(m_pVM, iField);
    }
}

int64_t CSQLite3Query::getInt64Field(const char* pszField, const int64_t iNullValue)
{
    int iField = fieldIndex(pszField);

    return getInt64Field(iField, iNullValue);
}

double CSQLite3Query::getFloatField(const int &iField, const double &fNullValue)
{
    if (SQLITE_NULL == fieldDataType(iField))
    {
        return fNullValue;
    }
    else
    {
        return sqlite3_column_double(m_pVM, iField);
    }
}


double CSQLite3Query::getFloatField(const char* pszField, const double fNullValue)
{
    int iField = fieldIndex(pszField);

    return getFloatField(iField, fNullValue);
}

const char* CSQLite3Query::getStringField(const int &iField, const char* pszNullValue)
{
    if (SQLITE_NULL == fieldDataType(iField))
    {
        return pszNullValue;
    }
    else
    {
        return (const char*)sqlite3_column_text(m_pVM, iField);
    }
}

const char* CSQLite3Query::getStringField(const char* pszField, const char* pszNullValue)
{
    int iField = fieldIndex(pszField);

    return getStringField(iField, pszNullValue);
}

const unsigned char* CSQLite3Query::getBlobField(const int &iField, int &iLen)
{
    checkVM();

    iLen = 0;

    if (iField < 0 || iField > m_iCols - 1)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "invalid field index requested, %d", iField);
    }

    iLen = sqlite3_column_bytes(m_pVM, iField);

    return (const unsigned char*)sqlite3_column_blob(m_pVM, iField);
}

const unsigned char* CSQLite3Query::getBlobField(const char* pszField, int &iLen)
{
    int iField = fieldIndex(pszField);

    return getBlobField(iField, iLen);
}

int CSQLite3Query::fieldIndex(const char* pszField)
{
    checkVM();

    if (pszField)
    {
        for (int iField = 0; iField < m_iCols; iField++)
        {
            const char* pszTemp = sqlite3_column_name(m_pVM, iField);
            if (0 == strcmp(pszField, pszTemp))
            {
                return iField;
            }
        }
    }

    Q_EXCEPTION(Q_ERROR_DATABASE, "invalid field name requested, %s", pszField);
}

int CSQLite3Query::fieldDataType(int iCol)
{
    checkVM();

    if (iCol < 0 || iCol > m_iCols-1)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "invalid field index requested, %d", iCol);
    }

    return sqlite3_column_type(m_pVM, iCol);
}

bool CSQLite3Query::eof(void)
{
    checkVM();

    return m_bEof;
}

void CSQLite3Query::nextRow(void)
{
    checkVM();

    int iRtn = sqlite3_step(m_pVM);

    if (SQLITE_DONE == iRtn)
    {
        // no rows
        m_bEof = true;
    }
    else if (SQLITE_ROW == iRtn)
    {
        // more rows, nothing to do
    }
    else
    {
        int iErrorCode = sqlite3_errcode(m_pDB);
        std::string strErrorMsg = sqlite3_errmsg(m_pDB);

        (void)sqlite3_finalize(m_pVM);
        m_pVM = NULL;

        Q_EXCEPTION(iErrorCode, "%s", strErrorMsg.c_str());
    }
}

void CSQLite3Query::freeRes(void)
{
    if (m_pVM && m_bOwnVM)
    {
        int iRtn = sqlite3_finalize(m_pVM);
        m_pVM = NULL;
        if (iRtn != SQLITE_OK)
        {
            Q_EXCEPTION(sqlite3_errcode(m_pDB), "%s", sqlite3_errmsg(m_pDB));
        }
    }
}

void CSQLite3Query::finalize(void)
{
    delete this;
}

void CSQLite3Query::checkVM(void)
{
    if (NULL == m_pVM)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "%s", Q_EXCEPTION_NULLPOINTER);
    }
}

/**********************************************************************/
CSQLite3Statement::CSQLite3Statement(void) : m_pDB(NULL), m_pVM(NULL), m_iCols(Q_INIT_NUMBER)
{

}

CSQLite3Statement::CSQLite3Statement(sqlite3* pDB, sqlite3_stmt* pVM)
{
    m_pDB = pDB;
    m_pVM = pVM;
    m_iCols =  sqlite3_bind_parameter_count(m_pVM);
}

CSQLite3Statement::~CSQLite3Statement(void)
{
    try
    {
        freeVM();
    }
    catch (...)
    {
    }
}

void CSQLite3Statement::finalize(void)
{
    delete this;
}

void CSQLite3Statement::freeVM(void)
{
    if (NULL != m_pVM)
    {
        int iRtn = sqlite3_finalize(m_pVM);
        m_pVM = NULL;
        if (iRtn != SQLITE_OK)
        {
            Q_EXCEPTION(sqlite3_errcode(m_pDB), "%s", sqlite3_errmsg(m_pDB));
        }
    }
}

void CSQLite3Statement::checkDB(void)
{
    if (NULL == m_pDB)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "%s", Q_EXCEPTION_NULLPOINTER);
    }
}

void CSQLite3Statement::checkVM(void)
{
    if (NULL == m_pVM)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "%s", Q_EXCEPTION_NULLPOINTER);
    }
}

void CSQLite3Statement::CheckParam(int iParam)
{
    if(iParam < 0
        || iParam >= m_iCols)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "invalid field index requested, %d", iParam);
    }
}

int CSQLite3Statement::execDML(void)
{
    checkDB();
    checkVM();

    int iRtn = sqlite3_step(m_pVM);
    if (SQLITE_DONE == iRtn)
    {
        int iRowsChanged = sqlite3_changes(m_pDB);

        iRtn = sqlite3_reset(m_pVM);
        if (iRtn != SQLITE_OK)
        {
            Q_EXCEPTION(sqlite3_errcode(m_pDB), "%s", sqlite3_errmsg(m_pDB));
        }

        return iRowsChanged;
    }
    else
    {
        int iErrorCode = sqlite3_errcode(m_pDB);
        std::string strErrorMsg = sqlite3_errmsg(m_pDB);

        (void)sqlite3_reset(m_pVM);

        Q_EXCEPTION(iErrorCode, "%s", strErrorMsg.c_str());
    }
}

CDBQuery *CSQLite3Statement::execQuery(void)
{
    checkDB();
    checkVM();

    CSQLite3Query *pQuery = NULL;

    int iRtn = sqlite3_step(m_pVM);
    if (SQLITE_DONE == iRtn)
    {
        try
        {
            pQuery = new CSQLite3Query(m_pDB, m_pVM, true, false);
        }
        catch(std::bad_alloc &)
        {
            Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s", Q_EXCEPTION_ALLOCMEMORY);
        }

        return pQuery;
    }
    else if (SQLITE_ROW == iRtn)
    {
        try
        {
            pQuery = new CSQLite3Query(m_pDB, m_pVM, false, false);
        }
        catch(std::bad_alloc &)
        {
            Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s", Q_EXCEPTION_ALLOCMEMORY);
        }

        return pQuery;
    }
    else
    {
        int iErrorCode = sqlite3_errcode(m_pDB);
        std::string strErrorMsg = sqlite3_errmsg(m_pDB);

        (void)sqlite3_reset(m_pVM);

        Q_EXCEPTION(iErrorCode, "%s", strErrorMsg.c_str());
    }
}

int CSQLite3Statement::bindParameterIndex(const char* pszParam)
{
    checkVM();

    int iParam = sqlite3_bind_parameter_index(m_pVM, pszParam);
    if (!iParam)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "parameter '%s' is not valid for this statement", pszParam);
    }

    return iParam;
}

void CSQLite3Statement::bindString(const int iField, const char* pszValue)
{
    checkVM();
    CheckParam(iField);

    int iRtn = sqlite3_bind_text(m_pVM, iField + 1, pszValue, -1, SQLITE_TRANSIENT);
    if (iRtn != SQLITE_OK)
    {
        Q_EXCEPTION(sqlite3_errcode(m_pDB), "%s", sqlite3_errmsg(m_pDB));
    }
}

void CSQLite3Statement::bindInt(const int iField, const int iValue)
{
    checkVM();
    CheckParam(iField);

    int iRtn = sqlite3_bind_int(m_pVM, iField + 1, iValue);
    if (iRtn != SQLITE_OK)
    {
        Q_EXCEPTION(sqlite3_errcode(m_pDB), "%s", sqlite3_errmsg(m_pDB));
    }
}

void CSQLite3Statement::bindInt64(const int iField, const int64_t iValue)
{
    checkVM();
    CheckParam(iField);

    int iRtn = sqlite3_bind_int64(m_pVM, iField + 1, iValue);
    if (iRtn != SQLITE_OK)
    {
        Q_EXCEPTION(sqlite3_errcode(m_pDB), "%s", sqlite3_errmsg(m_pDB));
    }
}

void CSQLite3Statement::bindFloat(const int iField, const double dValue)
{
    checkVM();
    CheckParam(iField);

    int iRtn = sqlite3_bind_double(m_pVM, iField + 1, dValue);
    if (iRtn != SQLITE_OK)
    {
        Q_EXCEPTION(sqlite3_errcode(m_pDB), "%s", sqlite3_errmsg(m_pDB));
    }
}

void CSQLite3Statement::bindBlob(const int iField, const unsigned char* blobValue, const size_t iLen)
{
    checkVM();
    CheckParam(iField);

    int iRtn = sqlite3_bind_blob(m_pVM, iField + 1,
        (const void*)blobValue, iLen, SQLITE_TRANSIENT);
    if (iRtn != SQLITE_OK)
    {
        Q_EXCEPTION(sqlite3_errcode(m_pDB), "%s", sqlite3_errmsg(m_pDB));
    }
}

void CSQLite3Statement::bindNull(const int iField)
{
    checkVM();
    CheckParam(iField);

    int iRtn = sqlite3_bind_null(m_pVM, iField + 1);
    if (iRtn != SQLITE_OK)
    {
        Q_EXCEPTION(sqlite3_errcode(m_pDB), "%s", sqlite3_errmsg(m_pDB));
    }
}

/**********************************************************************/
CSQLite3Link::CSQLite3Link(void)
{
    m_pDB = NULL;
    m_iBusyTimeoutMs = DBQUERY_TIMEOUT;
}

CSQLite3Link::~CSQLite3Link(void)
{
    try
    {
        close();
    }
    catch (...)
    {
    }    
}

void CSQLite3Link::open(CDBUrl &objDBUrl)
{
    int iRtn =  0;

    iRtn = sqlite3_open(objDBUrl.getDB().c_str(), &m_pDB);
    if (iRtn != SQLITE_OK)
    {
        Q_EXCEPTION(sqlite3_errcode(m_pDB), "%s", sqlite3_errmsg(m_pDB));
    }

    setBusyTimeout(m_iBusyTimeoutMs);
}

void CSQLite3Link::close(void)
{
    if (m_pDB)
    {
        if (SQLITE_OK == sqlite3_close(m_pDB))
        {
            m_pDB = NULL;
        }
        else
        {
            Q_EXCEPTION(Q_ERROR_DATABASE, "%s", "unable to close database.");
        }
    }
}

void CSQLite3Link::Transaction(void)
{
    (void)execDML("begin transaction;");
}

void CSQLite3Link::commitTransaction(void)
{
    (void)execDML("commit transaction;");
}

void CSQLite3Link::rollBack(void)
{
    (void)execDML("rollback transaction;");
}

int CSQLite3Link::execDML(const char* szSQL)
{
    if (NULL == szSQL)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "%s", Q_EXCEPTION_NULLPOINTER);
    }

    checkDB();

    char* pszError = NULL;
    std::string strTmp;

    int iRtn = sqlite3_exec(m_pDB, szSQL, 0, 0, &pszError);
    if (SQLITE_OK == iRtn)
    {
        iRtn = sqlite3_changes(m_pDB);

        return iRtn;
    }

    strTmp = std::string(pszError);
    sqlite3_free(pszError);

    Q_EXCEPTION(iRtn, "%s", strTmp.c_str());
}

CDBStatement *CSQLite3Link::compileStatement(const char* pszSQL)
{
    sqlite3_stmt* pVM = compile(pszSQL);
    CSQLite3Statement *pStatMem = NULL;

    try
    {
        pStatMem = new CSQLite3Statement(m_pDB, pVM);
    }
    catch(std::bad_alloc &)
    {
        (void)sqlite3_finalize(pVM);
        Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s", Q_EXCEPTION_ALLOCMEMORY);
    }

    return pStatMem;
}

CDBQuery* CSQLite3Link::execQuery(const char* szSQL)
{
    if (NULL == szSQL)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "%s", Q_EXCEPTION_NULLPOINTER);
    }

    checkDB();

    CSQLite3Query *pQuery = NULL;

    sqlite3_stmt* pVM = compile(szSQL);
    int iRtn = sqlite3_step(pVM);
    if (SQLITE_DONE == iRtn)
    {
        try
        {
            pQuery = new CSQLite3Query(m_pDB, pVM, true);
        }
        catch(std::bad_alloc &)
        {
            (void)sqlite3_finalize(pVM);
            Q_EXCEPTION(Q_ERROR_DATABASE, "%s", Q_EXCEPTION_ALLOCMEMORY);
        }

        return pQuery;
    }
    else if (SQLITE_ROW == iRtn)
    {
        // at least 1 row
        try
        {
            pQuery = new CSQLite3Query(m_pDB, pVM, false);
        }
        catch(std::bad_alloc &)
        {
            (void)sqlite3_finalize(pVM);
            Q_EXCEPTION(Q_ERROR_DATABASE, "%s", Q_EXCEPTION_ALLOCMEMORY);
        }

        return pQuery;
    }
    else
    {
        int iErrorCode = sqlite3_errcode(m_pDB);
        std::string strErrorMsg = sqlite3_errmsg(m_pDB);

        (void)sqlite3_finalize(pVM);

        Q_EXCEPTION(iErrorCode, "%s", strErrorMsg.c_str());
    }
}

void CSQLite3Link::setBusyTimeout(int nMillisecs)
{
    m_iBusyTimeoutMs = nMillisecs;
    sqlite3_busy_timeout(m_pDB, m_iBusyTimeoutMs);
}

void CSQLite3Link::checkDB()
{
    if (NULL == m_pDB)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "%s", Q_EXCEPTION_NULLPOINTER);
    }
}

sqlite3_stmt* CSQLite3Link::compile(const char* szSQL)
{
    if (NULL == szSQL)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "%s", Q_EXCEPTION_NULLPOINTER);
    }

    checkDB();

    const char* szTail=0;
    sqlite3_stmt* pVM;

    int iRtn = sqlite3_prepare_v2(m_pDB, szSQL, -1, &pVM, &szTail);
    if (iRtn != SQLITE_OK)
    {
        Q_EXCEPTION(sqlite3_errcode(m_pDB), "%s", sqlite3_errmsg(m_pDB));
    }

    return pVM;
}
