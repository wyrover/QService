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

#ifndef Q_DATABASE_H_
#define Q_DATABASE_H_

#include "QString.h"
#include "Exception.h"

#define DBQUERY_TIMEOUT 60000

//格式sql语句
class CSqlBuffer
{
public:
    CSqlBuffer(void);
    ~CSqlBuffer(void);

    /*sql语句格式化*/
    const char* Format(const char* szFormat, ...);

    operator const char*() { return m_pBuf;};

    void Clear(void);

private:
    char* m_pBuf;
};

//数据库连接信息
class CDBUrl
{
public:
    CDBUrl(void){};
    ~CDBUrl(void){};

    /*Url字串含义：
    *DB：数据库名
    *Host:主机名
    *UserName:用户名
    *PassWord:密码
    *Port:端口
    *Url格式：
    *DB=test;UserName=admin;PassWord=admin;Port=306;Host=www.baidu.com;
    */
    void Format(const char *pszFormat, ...);
    /*Url字串含义：
    *DB：数据库名
    *Host:主机名
    *UserName:用户名
    *PassWord:密码
    *Port:端口
    *Url格式：
    *DB=test;UserName=admin;PassWord=admin;Port=306;Host=www.baidu.com;
    */
    void setUrl(const char *pszUrl)
    {
        if (NULL == pszUrl)
        {
            m_strUrl.clear();

            return;
        }

        m_strUrl = pszUrl;
    };

    std::string getUrl(void)
    {
        return m_strUrl;
    };

    std::string getDB(void);
    std::string getHost(void);
    std::string getUserName(void);
    std::string getPassWord(void);
    int getPort(void);

private:
    std::string getInfo(const char *pszFlag);

private:
    std::string m_strUrl;
};

//查询
class CDBQuery
{
public:
    CDBQuery(void){};
    virtual ~CDBQuery(void){};

    virtual int getIntField(const char* pszField, const int iNullValue = 0)
    {
        return iNullValue;
    };

    virtual int64_t getInt64Field(const char* pszField, const int64_t iNullValue = 0)
    {
        return iNullValue;
    };

    std::string getStrInt64Field(const char* pszField)
    {
        return Q_ToString(getInt64Field(pszField));
    };

    virtual double getFloatField(const char* pszField, const double fNullValue = 0.0)
    {
        return fNullValue;
    };

    virtual const char* getStringField(const char* pszField, const char* pszNullValue = "")
    {
        return pszNullValue;
    };

    virtual const unsigned char* getBlobField(const char* pszField, int& iLen)
    {
        return NULL;
    };

    virtual bool eof(void)
    {
        return true;
    };

    virtual void nextRow(void){};
    virtual void finalize(void){};
};

//Statement
class CDBStatement
{
public:
    CDBStatement(void){};
    virtual ~CDBStatement(void){};

    virtual int execDML(void){return 0;};
    virtual CDBQuery *execQuery(void){return NULL;};

    virtual void bindString(const int iField, const char* pszValue){};
    virtual void bindInt(const int iField, const int iValue){};
    virtual void bindInt64(const int iField, const int64_t iValue){};
    void bindStrInt64(const int iField, const char *pszValue)
    {
        assert(NULL != pszValue);

        bindInt64(iField, Q_ToNumber<int64_t>(pszValue));
    };
    virtual void bindFloat(const int iField, const double dValue){};
    virtual void bindBlob(const int iField, const unsigned char* blobValue, const size_t iLen){};
    virtual void bindNull(const int iField){};
    virtual void clearParam(void){};
    virtual void finalize(void){};
};

//连接
class CDBLink
{
public:
    CDBLink(void):m_bUsed(false){};
    virtual ~CDBLink(void){};

    virtual void open(CDBUrl &objDBUrl){};
    virtual void close(void){};

    virtual void Transaction(void){};
    virtual void commitTransaction(void){};
    virtual void rollBack(void){};

    virtual int execDML(const char* szSQL){return -1;};
    virtual CDBQuery* execQuery(const char* pszSQL)
    {
        return NULL;
    };
    virtual CDBStatement *compileStatement(const char* pszSQL){return NULL;};

    virtual int reLink(void){return Q_RTN_OK;};

    virtual void setBusyTimeout(int nMillisecs){};

    void setUsed(bool bUsed)
    {
        m_bUsed = bUsed;
    };
    bool getUsed(void)
    {
        return m_bUsed;
    };

private:
    bool m_bUsed;
};

#endif//Q_DATABASE_H_
