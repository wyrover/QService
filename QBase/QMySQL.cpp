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

#include "QMySQL.h"

#ifdef Q_WITHMYSQL

class CMySqlLibInit
{
public:
    CMySqlLibInit(void)
    {
        if (Q_RTN_OK != mysql_library_init(0, NULL, NULL))
        {
            Q_EXCEPTION(Q_RTN_ERROR, "%s", "init mysql library error.");
        }
    };
    ~CMySqlLibInit(void)
    {
        mysql_library_end();
    };
};

static CMySqlLibInit objMySqlInit;

bool CheckProcedure(const char *pszSql)
{
    std::string strSql = pszSql;

    strSql = Q_Trim(strSql);
    if (strSql.size() < strlen("CALL"))
    {
        return false;
    }

    strSql = Q_Convert2Upper(strSql);
    if (std::string("CALL") == strSql.substr(0, strlen("CALL")))
    {
        return true;
    }

    return false;
}

CMySQLQuery::CMySQLQuery(void)
{
    m_pDB = NULL;
    m_Mysql_Res = NULL;
    m_Field = NULL;
    m_Row = NULL;
    m_Row_Count = 0;
    m_Field_Count = 0;
}

CMySQLQuery::CMySQLQuery(MYSQL_RES *mysql_res, MYSQL *pDB)
{
    m_pDB = pDB;
    setMysqlRst(mysql_res);
}

CMySQLQuery::~CMySQLQuery(void)
{
    if (m_Mysql_Res != NULL)
    {
        mysql_free_result(m_Mysql_Res);
        m_Mysql_Res = NULL;
    }
}

void CMySQLQuery::finalize(void)
{
    delete this;
}

void CMySQLQuery::setMysqlRst(MYSQL_RES *mysql_res)
{
    m_Mysql_Res = mysql_res;

    checkVM();

    //定位游标位置到第一个位置
    mysql_data_seek(m_Mysql_Res, 0);
    m_Row =  mysql_fetch_row(m_Mysql_Res);
    m_Row_Count = (unsigned long)mysql_num_rows(m_Mysql_Res); 
    //得到字段数量
    m_Field_Count = mysql_num_fields(m_Mysql_Res);
}

int CMySQLQuery::fieldIndex(const char* pszField)
{
    if (NULL == pszField)
    {
        Q_EXCEPTION(Q_ERROR_NULLPOINTER, "%s", Q_EXCEPTION_NULLPOINTER);
    }

    checkVM();

    unsigned int i = 0;

    mysql_field_seek(m_Mysql_Res, 0);//定位到第0列
    while (i < m_Field_Count)
    {
        m_Field = mysql_fetch_field(m_Mysql_Res);
        if ((m_Field != NULL) 
            && (0 == strcmp(m_Field->name, pszField)))//找到
        {
            return i;
        }

        i++;
    }

    Q_EXCEPTION(Q_ERROR_DATABASE, "invalid field name requested, %s", pszField);
}

int CMySQLQuery::getIntField(const int &iField, const int &iNullValue)
{
    const char *pszVal = getStringField(iField, "");
    if (NULL == pszVal
        || 0 == strlen(pszVal))
    {
        return iNullValue;
    }

    return atoi(pszVal);
}

int CMySQLQuery::getIntField(const char* pszField, const int iNullValue)
{
    int iField = fieldIndex(pszField);

    return getIntField(iField, iNullValue);
}

int64_t CMySQLQuery::getInt64Field(const int &iField, const int64_t &iNullValue)
{
    const char *pszVal = getStringField(iField, "");
    if (NULL == pszVal
        || 0 == strlen(pszVal))
    {
        return iNullValue;
    }
    
    int64_t  iValue = 0;
    std::stringstream strValue;

    strValue << pszVal;
    strValue >> iValue;
    
    return iValue;
}

int64_t CMySQLQuery::getInt64Field(const char* pszField, const int64_t iNullValue)
{
    int iField = fieldIndex(pszField);

    return getInt64Field(iField, iNullValue);
}

double CMySQLQuery::getFloatField(const int &iField, const double &fNullValue)
{
    const char *pszVal = getStringField(iField, "");
    if (NULL == pszVal
        || 0 == strlen(pszVal))
    {
        return fNullValue;
    }

    return atof(pszVal);
}

double CMySQLQuery::getFloatField(const char* pszField, const double fNullValue)
{
    int iField = fieldIndex(pszField);

    return getFloatField(iField, fNullValue);
}

const char* CMySQLQuery::getStringField(const int &iField, const char* pszNullValue)
{
    checkVM();

    if (iField < 0
        || iField  >= (int)m_Field_Count)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "invalid field index requested, %d", iField);
    }

    if (NULL == m_Row)
    {
        return pszNullValue;
    }

    if (NULL == m_Row[iField])
    {
        return pszNullValue;
    }

    return m_Row[iField];
}

const char* CMySQLQuery::getStringField(const char* pszField, const char* pszNullValue)
{
    int iField = fieldIndex(pszField);

    return getStringField(iField, pszNullValue);
}

const unsigned char* CMySQLQuery::getBlobField(const int &iField, int &iLen)
{
    checkVM();

    if (iField < 0
        || iField  >= (int)m_Field_Count)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "invalid field index requested, %d", iField);
    }

    if (NULL == m_Row)
    {
        return NULL;
    }

    unsigned long *pLens = mysql_fetch_lengths(m_Mysql_Res);
    iLen = (int)pLens[iField];

    return (const unsigned char *)m_Row[iField];
}

const unsigned char* CMySQLQuery::getBlobField(const char* pszField, int &iLen)
{
    int iField = fieldIndex(pszField);

    return getBlobField(iField, iLen);
}

void CMySQLQuery::nextRow(void)
{
    checkVM();
    m_Row = mysql_fetch_row(m_Mysql_Res);
}

bool CMySQLQuery::eof(void)
{
    if (NULL == m_Row)
    {
        return true;
    }

    return false;
}

void CMySQLQuery::checkVM(void)
{
    if (NULL == m_Mysql_Res)
    {
        Q_EXCEPTION(Q_ERROR_NULLPOINTER, "%s", Q_EXCEPTION_NULLPOINTER);
    }
}

/**********************************************************************/
CMySQLStmtQuery::CMySQLStmtQuery(void) : m_pRstBinder(NULL), m_pStBindInfo(NULL), 
    m_pStmt(NULL), m_pDB(NULL), m_uiCol(Q_INIT_NUMBER)
{

}

CMySQLStmtQuery::CMySQLStmtQuery(MYSQL_BIND *pRstBinder, BindInfo *pStBindInfo, 
    MYSQL_STMT *pStmt, unsigned int uiCol, MYSQL *pDB)
{
    m_pDB = pDB;
    m_pRstBinder = pRstBinder;
    m_pStBindInfo = pStBindInfo;
    m_pStmt = pStmt;
    m_uiCol = uiCol;
}

CMySQLStmtQuery::~CMySQLStmtQuery(void)
{
    freeRes();
}

void CMySQLStmtQuery::freeRes(void)
{
    char *pTmp;

    for (unsigned int i = 0; i < m_uiCol; i++)
    {
        if (NULL != m_pRstBinder[i].length)
        {
            Q_SafeDelete(m_pRstBinder[i].length);
        }
        if (NULL != m_pRstBinder[i].buffer)
        {
            pTmp = (char*)m_pRstBinder[i].buffer;
            Q_SafeDelete(pTmp);
            m_pRstBinder[i].buffer = NULL;
        }
    }

    Q_SafeDelete_Array(m_pStBindInfo);
    Q_SafeDelete_Array(m_pRstBinder);

    (void)mysql_stmt_free_result(m_pStmt);
}

void CMySQLStmtQuery::finalize(void)
{
    delete this;
}

int CMySQLStmtQuery::fieldIndex(const char* pszField)
{
    if (NULL == pszField)
    {
        Q_EXCEPTION(Q_ERROR_NULLPOINTER, "%s", Q_EXCEPTION_NULLPOINTER);
    }

    for (unsigned int i = 0; i < m_uiCol; i++)
    {
        if (std::string(pszField) == m_pStBindInfo[i].strFieldName)
        {
            return m_pStBindInfo[i].iIndex;
        }
    }

    Q_EXCEPTION(Q_ERROR_DATABASE, "invalid field name requested, %s", pszField);
}

void CMySQLStmtQuery::CheckIndex(int iField)
{
    if (iField < 0
        || iField  >= (int)m_uiCol)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "invalid field index requested, %d", iField);
    }
}

bool CMySQLStmtQuery::eof(void)
{
    int iRtn = mysql_stmt_fetch(m_pStmt);

    return (Q_RTN_OK == iRtn) ? false : true;
}

int64_t CMySQLStmtQuery::getInt64Field(const int &iField, const int64_t &iNullValue)
{
    CheckIndex(iField);

    MYSQL_BIND &stData = m_pRstBinder[iField];

    switch (stData.buffer_type)
    {
    case MYSQL_TYPE_LONGLONG:
        {
            return *(long long*)(stData.buffer);
        }
        break;

    case MYSQL_TYPE_LONG:
        {
            return *(int*)(stData.buffer);
        }
        break;

    case MYSQL_TYPE_SHORT:
        {
            return *(short*)(stData.buffer);
        }
        break;

    default:
        {
            return iNullValue;
        }
        break;
    }
}

int CMySQLStmtQuery::getIntField(const char* pszField, const int iNullValue)
{
    int iField = fieldIndex(pszField);

    return (int)getInt64Field(iField, iNullValue);
}

int64_t CMySQLStmtQuery::getInt64Field(const char* pszField, const int64_t iNullValue)
{
    int iField = fieldIndex(pszField);

    return getInt64Field(iField, iNullValue);
}

double CMySQLStmtQuery::getFloatField(const int &iField, const double &dNullValue)
{
    CheckIndex(iField);

    MYSQL_BIND &stData = m_pRstBinder[iField];

    switch (stData.buffer_type)
    {
    case MYSQL_TYPE_DOUBLE:
        {
            return *(double*)(stData.buffer);
        }
        break;

    case MYSQL_TYPE_FLOAT:
        {
            return *(float*)(stData.buffer);
        }
        break;

    default:
        {
            return dNullValue;
        }
        break;
    }
}

double CMySQLStmtQuery::getFloatField(const char* pszField, const double fNullValue)
{
    int iField = fieldIndex(pszField);
    
    return getFloatField(iField, fNullValue);
}

const char* CMySQLStmtQuery::getStringField(const int &iField, const char* pszNullValue)
{
    CheckIndex(iField);

    MYSQL_BIND &stData = m_pRstBinder[iField];

    switch (stData.buffer_type)
    {
    case MYSQL_TYPE_VAR_STRING:
    case MYSQL_TYPE_STRING:
        {
            return (const char*)(stData.buffer);
        }
        break;

    default:
        {
            return pszNullValue;
        }
        break;
    }
}

const char* CMySQLStmtQuery::getStringField(const char* pszField, const char* pszNullValue)
{
    int iField = fieldIndex(pszField);

    return getStringField(iField, pszNullValue);
}

const unsigned char* CMySQLStmtQuery::getBlobField(const int &iField, int &iLen)
{
    CheckIndex(iField);

    MYSQL_BIND &stData = m_pRstBinder[iField];
    iLen = 0;

    switch (stData.buffer_type)
    {
    case MYSQL_TYPE_TINY_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:
    case MYSQL_TYPE_BLOB:
        {
            iLen =  (int)(*(stData.length));

            return (const unsigned char*)(stData.buffer);
        }
        break;

    default:
        {
            return NULL;
        }
        break;
    }

    return  (const unsigned char*)m_pRstBinder[iField].buffer;
}

const unsigned char* CMySQLStmtQuery::getBlobField(const char* pszField, int &iLen)
{
    int iField = fieldIndex(pszField);

    return getBlobField(iField, iLen);
}

/**********************************************************************/
CMySQLStatement::CMySQLStatement(void) : m_pDB(NULL), m_pStmt(NULL), 
    m_pBinder_Param(NULL), m_pMysql_Res(NULL), m_pField(NULL), m_pBindMemInfo(NULL),
    m_uiParamNum(Q_INIT_NUMBER), m_uiCol(Q_INIT_NUMBER), m_bProcedure(false)
{

}

CMySQLStatement::CMySQLStatement(MYSQL *pDB, MYSQL_STMT *pStmt, bool bProcedure)
{
    m_pDB = NULL;
    m_pStmt = NULL;
    m_pBinder_Param = NULL;
    m_pMysql_Res = NULL;
    m_pField = NULL;
    m_pBindMemInfo = NULL;
    m_uiCol = Q_INIT_NUMBER;
    m_pDB = pDB;
    m_pStmt = pStmt;
    m_bProcedure = bProcedure;

    //参数个数
    m_uiParamNum = (unsigned int)mysql_stmt_param_count(m_pStmt);
    if (m_uiParamNum <= 0)
    {
        return;
    }

    m_pBinder_Param = new(std::nothrow) MYSQL_BIND[m_uiParamNum];
    if (NULL == m_pBinder_Param)
    {
        Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s size %u", Q_EXCEPTION_ALLOCMEMORY, m_uiParamNum);
    }

    Q_Zero(m_pBinder_Param, m_uiParamNum * sizeof(MYSQL_BIND));

    m_pBindMemInfo = new(std::nothrow) BindMemInfo[m_uiParamNum];
    if (NULL == m_pBindMemInfo)
    {
        Q_SafeDelete_Array(m_pBinder_Param);

        Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s size %u", Q_EXCEPTION_ALLOCMEMORY, m_uiParamNum);
    }
}

void CMySQLStatement::getFiledInfo(void)
{
    unsigned int uiFieldCond = Q_INIT_NUMBER;
    m_pField = Q_INIT_NUMBER;
    m_uiCol = Q_INIT_NUMBER;
    m_pMysql_Res = NULL;

    //字段个数
    uiFieldCond = mysql_stmt_field_count(m_pStmt);
    if (uiFieldCond <= 0)
    {
        return;
    }

    m_pMysql_Res = mysql_stmt_result_metadata(m_pStmt);
    if (NULL != m_pMysql_Res)
    {
        m_pField = mysql_fetch_fields(m_pMysql_Res);
        m_uiCol = mysql_num_fields(m_pMysql_Res);

        mysql_free_result(m_pMysql_Res);
    }
}

CMySQLStatement::~CMySQLStatement(void)
{
    try
    {
        freeVM();
    }
    catch (...)
    {
    }
}

void CMySQLStatement::finalize(void)
{
    delete this;
}

void CMySQLStatement::freeVM(void)
{
    if (NULL != m_pStmt)
    {
        (void)mysql_stmt_close(m_pStmt);
        m_pStmt = NULL;
    }

    Q_SafeDelete_Array(m_pBinder_Param);
    clearParam();
    Q_SafeDelete_Array(m_pBindMemInfo);
}

void CMySQLStatement::clearParam(void)
{
    if (NULL == m_pBindMemInfo)
    {
        return;
    }

    for (unsigned int i = 0; i < m_uiParamNum; i++)
    {
        Q_SafeDelete(m_pBindMemInfo[i].pMem);
    }
}

void CMySQLStatement::checkDB(void)
{
    if (NULL == m_pDB)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "%s", Q_EXCEPTION_NULLPOINTER);
    }
}

void CMySQLStatement::checkVM(void)
{
    if (NULL == m_pStmt)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "%s", Q_EXCEPTION_NULLPOINTER);
    }
}

void CMySQLStatement::checkBinder(void)
{
    if (NULL == m_pBinder_Param)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "%s", Q_EXCEPTION_NULLPOINTER);
    }
}

void CMySQLStatement::CheckParam(int iParam)
{
    if (iParam < 0
        || iParam >= (int)m_uiParamNum)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "param error. %d", iParam);
    }
}

void CMySQLStatement::BindParam(void)
{
    if (NULL == m_pBinder_Param)
    {
        return;
    }

    if (Q_RTN_OK != mysql_stmt_bind_param(m_pStmt, m_pBinder_Param))
    {
        Q_EXCEPTION(mysql_stmt_errno(m_pStmt), "%s", mysql_stmt_error(m_pStmt));
    }
}

int CMySQLStatement::execDML(void)
{
    checkDB();
    checkVM();

    int iChangeVal = Q_INIT_NUMBER;

    reSet();
    BindParam();

    if (Q_RTN_OK != mysql_stmt_execute(m_pStmt))
    {
        Q_EXCEPTION(mysql_stmt_errno(m_pStmt), "%s", mysql_stmt_error(m_pStmt));
    }

    iChangeVal = (int)mysql_stmt_affected_rows(m_pStmt);
    mysql_stmt_store_result(m_pStmt);
    mysql_stmt_free_result(m_pStmt);

    return iChangeVal;
}

CDBQuery *CMySQLStatement::execQuery(void)
{
    checkDB();
    checkVM();

    MYSQL_BIND *pRstBinder = NULL;
    BindInfo *pStBindInfo = NULL;
    CMySQLStmtQuery *pQuery = NULL;

    reSet();
    BindParam();

    if (Q_RTN_OK != mysql_stmt_execute(m_pStmt))
    {
        Q_EXCEPTION(mysql_stmt_errno(m_pStmt), "%s", mysql_stmt_error(m_pStmt));
    }

    getFiledInfo();
    if (0 >= m_uiCol)
    {
        Q_EXCEPTION(Q_ERROR_DATABASE, "%s", "no field");
    }

    pStBindInfo = new(std::nothrow) BindInfo[m_uiCol];
    if (NULL == pStBindInfo)
    {
        clearExecQuery(pRstBinder, pStBindInfo);

        Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s size %u", Q_EXCEPTION_ALLOCMEMORY, m_uiCol);
    }

    pRstBinder = new(std::nothrow) MYSQL_BIND[m_uiCol];
    if (NULL == pRstBinder)
    {
        clearExecQuery(pRstBinder, pStBindInfo);

        Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s size %u", Q_EXCEPTION_ALLOCMEMORY, m_uiCol);
    }

    Q_Zero(pRstBinder, m_uiCol * sizeof(MYSQL_BIND));

    getRstBinderInfo(pRstBinder, pStBindInfo);

    if (Q_RTN_OK != mysql_stmt_bind_result(m_pStmt, pRstBinder))
    {
        clearExecQuery(pRstBinder, pStBindInfo);

        Q_EXCEPTION(mysql_stmt_errno(m_pStmt), "%s", mysql_stmt_error(m_pStmt));
    }
    
    if (Q_RTN_OK != mysql_stmt_store_result(m_pStmt))
    {
        clearExecQuery(pRstBinder, pStBindInfo);

        Q_EXCEPTION(mysql_stmt_errno(m_pStmt), "%s", mysql_stmt_error(m_pStmt));
    }

    try
    {
        if (m_bProcedure)
        {
            pQuery = new CMySQLStmtQuery(pRstBinder, pStBindInfo, m_pStmt, m_uiCol, m_pDB);
        }
        else
        {
            pQuery = new CMySQLStmtQuery(pRstBinder, pStBindInfo, m_pStmt, m_uiCol, NULL);
        }
    }
    catch(std::bad_alloc &)
    {
        clearExecQuery(pRstBinder, pStBindInfo);

        Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s", Q_EXCEPTION_ALLOCMEMORY);
    }

    return pQuery;
}

void CMySQLStatement::clearExecQuery(MYSQL_BIND *pBinder, BindInfo *pStBindInfo)
{
    if (NULL != pBinder)
    {
        char *pTmp;

        for (unsigned int i = 0; i < m_uiCol; i++)
        {
            if (NULL != pBinder[i].length)
            {
                Q_SafeDelete(pBinder[i].length);
            }
            if (NULL != pBinder[i].buffer)
            {
                pTmp = (char*)pBinder[i].buffer;
                Q_SafeDelete(pTmp);
                pBinder[i].buffer = NULL;
            }
        }
    }

    Q_SafeDelete_Array(pBinder);
    Q_SafeDelete_Array(pStBindInfo);

    (void)mysql_stmt_free_result(m_pStmt);
}

void CMySQLStatement::getRstBinderInfo(MYSQL_BIND *pBinder, BindInfo *pStBindInfo)
{ 
    for (unsigned int i = 0; i < m_uiCol; i++)
    {
        MYSQL_FIELD &stField = m_pField[i];
        MYSQL_BIND &stBinder = pBinder[i];
        BindInfo &stBindInfo = pStBindInfo[i];

        stBindInfo.iIndex = i;
        stBindInfo.strFieldName = stField.name;

        stBinder.buffer_type = stField.type;
        stBinder.buffer_length = stField.length;
        
        if (MYSQL_TYPE_TINY_BLOB == stBinder.buffer_type
            || MYSQL_TYPE_MEDIUM_BLOB == stBinder.buffer_type
            || MYSQL_TYPE_LONG_BLOB == stBinder.buffer_type
            || MYSQL_TYPE_BLOB == stBinder.buffer_type)
        {
            stBinder.length = new(std::nothrow) unsigned long;
            if (NULL == stBinder.length)
            {
                clearExecQuery(pBinder, pStBindInfo);

                Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s", Q_EXCEPTION_ALLOCMEMORY);
            }

            *(stBinder.length) = 0;
        }

        stBinder.buffer = new(std::nothrow) char[stBinder.buffer_length];
        if (NULL == stBinder.buffer)
        {
            clearExecQuery(pBinder, pStBindInfo);

            Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s size %u", 
                Q_EXCEPTION_ALLOCMEMORY, stBinder.buffer_length);
        }

        Q_Zero(stBinder.buffer, stBinder.buffer_length);
    }
}

void CMySQLStatement::setBindInfo(size_t iLens, BindMemInfo &stInfo)
{
    if (NULL == stInfo.pMem
        || iLens > stInfo.iLens)
    {
        Q_SafeDelete(stInfo.pMem);

        stInfo.pMem = new(std::nothrow) char[iLens];
        if (NULL == stInfo.pMem)
        {
            Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s size %u", Q_EXCEPTION_ALLOCMEMORY, iLens);
        }

        stInfo.iLens = iLens;
    }
}

void CMySQLStatement::bindString(const int iField, const char* pszValue)
{
    checkDB();
    checkVM();
    CheckParam(iField);

    size_t iLens = strlen(pszValue);
    BindMemInfo &stBindInfo = m_pBindMemInfo[iField];

    setBindInfo(iLens + 1, stBindInfo);
    memcpy(stBindInfo.pMem, pszValue, iLens);

    m_pBinder_Param[iField].buffer = (void*)stBindInfo.pMem;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_STRING;
    m_pBinder_Param[iField].buffer_length = iLens;
}

void CMySQLStatement::bindInt(const int iField, const int iValue)
{
    checkDB();
    checkVM();
    CheckParam(iField);

    size_t iLens = sizeof(iValue);
    BindMemInfo &stBindInfo = m_pBindMemInfo[iField];

    setBindInfo(iLens, stBindInfo);
    memcpy(stBindInfo.pMem, &iValue, iLens);

    m_pBinder_Param[iField].buffer = (void*)stBindInfo.pMem;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_LONG;
    m_pBinder_Param[iField].buffer_length = iLens;
}

void CMySQLStatement::bindInt64(const int iField, const int64_t iValue)
{
    checkDB();
    checkVM();
    CheckParam(iField);

    size_t iLens = sizeof(iValue);
    BindMemInfo &stBindInfo = m_pBindMemInfo[iField];

    setBindInfo(iLens, stBindInfo);
    memcpy(stBindInfo.pMem, &iValue, iLens);

    m_pBinder_Param[iField].buffer = (void*)stBindInfo.pMem;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_LONGLONG;
    m_pBinder_Param[iField].buffer_length = iLens;
}

void CMySQLStatement::bindFloat(const int iField, const double dValue)
{
    checkDB();
    checkVM();
    CheckParam(iField);

    size_t iLens = sizeof(dValue);
    BindMemInfo &stBindInfo = m_pBindMemInfo[iField];

    setBindInfo(iLens, stBindInfo);
    memcpy(stBindInfo.pMem, &dValue, iLens);

    m_pBinder_Param[iField].buffer = (void*)stBindInfo.pMem;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_DOUBLE;
    m_pBinder_Param[iField].buffer_length = iLens;
}

void CMySQLStatement::bindBlob(const int iField, const unsigned char* blobValue, const size_t iLen)
{
    checkDB();
    checkVM();
    CheckParam(iField);

    BindMemInfo &stBindInfo = m_pBindMemInfo[iField];

    setBindInfo(iLen + 1, stBindInfo);
    memcpy(stBindInfo.pMem, blobValue, iLen);

    m_pBinder_Param[iField].buffer = (void*)stBindInfo.pMem;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_BLOB;
    m_pBinder_Param[iField].buffer_length = iLen;
}

void CMySQLStatement::bindNull(const int iField)
{
    checkDB();
    checkVM();
    CheckParam(iField);

    m_pBinder_Param[iField].buffer = NULL;
    m_pBinder_Param[iField].buffer_type = MYSQL_TYPE_NULL;
    m_pBinder_Param[iField].buffer_length = 0;
}

void CMySQLStatement::reSet(void)
{
    if (NULL != m_pStmt)
    {
        int iRtn = mysql_stmt_reset(m_pStmt);
        if (iRtn != Q_RTN_OK)
        {
            Q_EXCEPTION(mysql_stmt_errno(m_pStmt), "%s", mysql_stmt_error(m_pStmt));
        }        
    }
}

/**********************************************************************/
CMySQLLink::CMySQLLink(void)
{
    m_pDb_Ptr = NULL;
    m_iBusyTimeoutMs = DBQUERY_TIMEOUT;
}

CMySQLLink::~CMySQLLink(void)
{
    close();
}

void CMySQLLink::close(void)
{
    if (m_pDb_Ptr != NULL)
    {
        mysql_close(m_pDb_Ptr);
        m_pDb_Ptr = NULL;
    }
}

void CMySQLLink::open(CDBUrl objDBUrl)
{
    int iRtn = Q_RTN_FAILE;
    char cReConnect = 1;

    m_pDb_Ptr = mysql_init(NULL);
    if (NULL == m_pDb_Ptr) 
    {
        goto EXT;
    }

    setBusyTimeout(m_iBusyTimeoutMs);

    //set re-conn to true. could use ping to reconn
    if (Q_RTN_OK != mysql_options(m_pDb_Ptr, MYSQL_OPT_RECONNECT, &cReConnect))
    {
        goto EXT;
    }

    //如果连接失败，返回NULL。对于成功的连接，返回值与第1个参数的值相同。
    if (NULL == mysql_real_connect(m_pDb_Ptr, 
        objDBUrl.getHost().c_str(), objDBUrl.getUserName().c_str(), 
        objDBUrl.getPassWord().c_str(), objDBUrl.getDB().c_str(), 
        objDBUrl.getPort(), NULL, CLIENT_MULTI_STATEMENTS ))
    {
        goto EXT;
    }

    if (Q_RTN_OK != mysql_options(m_pDb_Ptr, MYSQL_OPT_RECONNECT, &cReConnect))
    {
        goto EXT;
    }

    /*if (Q_RTN_OK != mysql_options(m_pDb_Ptr, MYSQL_SET_CHARSET_NAME, "utf8"))
    {
        goto EXT;
    }*/

    //选择制定的数据库失败
    //0表示成功，非0值表示出现错误。
    if (mysql_select_db( m_pDb_Ptr, objDBUrl.getDB().c_str()) != Q_RTN_OK) 
    {
        mysql_close(m_pDb_Ptr);
        m_pDb_Ptr = NULL;
        goto EXT;
    }

    /*execDML("SET NAMES `utf8`");
    execDML("SET CHARACTER SET `utf8`");*/
    
    iRtn = Q_RTN_OK;
EXT:
    //初始化mysql结构失败    
    if ( iRtn != Q_RTN_OK && m_pDb_Ptr != NULL )
    {
        std::string strTmp;
        unsigned int uiErrorCode = Q_RTN_OK;

        uiErrorCode = mysql_errno(m_pDb_Ptr);
        strTmp = mysql_error(m_pDb_Ptr);
        mysql_close(m_pDb_Ptr);
        m_pDb_Ptr = NULL;

        Q_EXCEPTION(uiErrorCode, "%s", strTmp.c_str());
    }
}

CDBQuery* CMySQLLink::execQuery(const char* szSQL)
{
    checkDB();

    if (NULL == szSQL)
    {
        Q_EXCEPTION(Q_ERROR_NULLPOINTER, "%s", Q_EXCEPTION_NULLPOINTER);
    }

    CMySQLQuery *pobj = NULL;

    int iRtn = mysql_real_query(m_pDb_Ptr, szSQL, strlen(szSQL));    
    if (Q_RTN_OK == iRtn)
    {
        MYSQL_RES *Mysql_Res = mysql_store_result(m_pDb_Ptr);

        try
        {
            if (CheckProcedure(szSQL))
            {
                pobj = new CMySQLQuery(Mysql_Res, m_pDb_Ptr); 
            }
            else
            {
                pobj = new CMySQLQuery(Mysql_Res, NULL); 
            }           

            return pobj;
        }
        catch(std::bad_alloc &)
        {
            mysql_free_result(Mysql_Res);
            Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s", Q_EXCEPTION_ALLOCMEMORY);
        }
    }

    Q_EXCEPTION(mysql_errno(m_pDb_Ptr), "%s", mysql_error(m_pDb_Ptr));
}

CDBStatement *CMySQLLink::compileStatement(const char* pszSQL)
{
    checkDB();

    MYSQL_STMT *pStmt = NULL;

    pStmt = mysql_stmt_init(m_pDb_Ptr);
    if (NULL == pStmt)
    {
        Q_EXCEPTION(mysql_errno(m_pDb_Ptr), "%s", mysql_error(m_pDb_Ptr));
    }
    
    int iStatus = mysql_stmt_prepare(pStmt, pszSQL, strlen(pszSQL));
    if (Q_RTN_OK == iStatus)
    {
        CMySQLStatement *pStatement = NULL;

        try
        {
            if (CheckProcedure(pszSQL))
            {
                pStatement = new CMySQLStatement(m_pDb_Ptr, pStmt, true);
            }
            else
            {
                pStatement = new CMySQLStatement(m_pDb_Ptr, pStmt, false);
            } 
            
        }
        catch(std::bad_alloc &)
        {
            mysql_stmt_close(pStmt);
            Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s", Q_EXCEPTION_ALLOCMEMORY);
        }

        return pStatement;
    }

    unsigned int uiErrorCode = mysql_stmt_errno(pStmt);
    std::string strError = mysql_stmt_error(pStmt);
    mysql_stmt_close(pStmt);

    Q_EXCEPTION(uiErrorCode, "%s", strError.c_str());
}

int CMySQLLink::execDML(const char* szSQL)
{
    checkDB();

    if (NULL == szSQL)
    {
        Q_EXCEPTION(Q_ERROR_NULLPOINTER, "%s", Q_EXCEPTION_NULLPOINTER);
    }

    int iRtn = mysql_real_query(m_pDb_Ptr, szSQL, strlen(szSQL));    
    if (Q_RTN_OK == iRtn)
    {
        //得到受影响的行数
        iRtn = (int)mysql_affected_rows(m_pDb_Ptr);
        MYSQL_RES *Mysql_Res = mysql_store_result(m_pDb_Ptr);
        if(NULL != Mysql_Res)
        {
            mysql_free_result(Mysql_Res);
        }

        return iRtn;
    }

    //执行查询失败
    Q_EXCEPTION(mysql_errno(m_pDb_Ptr), "%s", mysql_error(m_pDb_Ptr));
}

int CMySQLLink::reLink(void)
{
    if (NULL == m_pDb_Ptr)
    {
        return Q_RTN_FAILE;
    }

    return mysql_ping(m_pDb_Ptr);
}

void CMySQLLink::Transaction(void)
{
    (void)execDML("START TRANSACTION");
}

void CMySQLLink::commitTransaction(void)
{
    (void)execDML("COMMIT");
}

void CMySQLLink::rollBack(void)
{
    (void)execDML("ROLLBACK");
}

void CMySQLLink::setBusyTimeout(int nMillisecs)
{
    checkDB();
    m_pDb_Ptr->options.read_timeout = nMillisecs;
    m_pDb_Ptr->options.write_timeout = nMillisecs;
    m_pDb_Ptr->options.connect_timeout = nMillisecs;
}

void CMySQLLink::checkDB(void)
{
    if (NULL == m_pDb_Ptr)
    {
        Q_EXCEPTION(Q_ERROR_NULLPOINTER, "%s", Q_EXCEPTION_NULLPOINTER);
    }
}

#endif
