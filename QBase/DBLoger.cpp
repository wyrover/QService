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

#include "DBLoger.h"
#include "SysLog.h"
#include "InitSock.h"

#define LOG_TABLE "log"

CDBLoger::CDBLoger(void) : m_pStatement(NULL)
{
    m_strTable = getTableNam();
}

CDBLoger::~CDBLoger(void)
{
    if (NULL != m_pStatement)
    {
        m_pStatement->finalize();
        m_pStatement = NULL;
    }

    m_objLinker.close();
}

std::string CDBLoger::getTableNam(void)
{
    time_t now = time(NULL);
    struct tm *pstTM =localtime(&now);

    return Q_FormatStr("%s_%d_%d", LOG_TABLE, pstTM->tm_year + 1900, pstTM->tm_mon + 1);
}

bool CDBLoger::Init(const char *pszIp, unsigned short &usPort, 
    const char *pszUser, const char *pszPWD, const char *pszDB)
{
    m_objUrl.Format("DB=%s;UserName=%s;PassWord=%s;Port=%d;Host=%s;", 
        pszDB, pszUser, pszPWD, usPort, pszIp);

    if (Link())
    {
        return createTable(m_strTable);
    }

    return false;
}

void CDBLoger::writeDBLog(Q_SOCK &fd, const char *pszPlayerID, const short sType, 
    const char *pszMsg, const size_t iLens)
{
    if (Q_INVALID_SOCK == fd)
    {
        return;
    }

    int64_t iPlayerID = Q_ToNumber<int64_t>(pszPlayerID);

    size_t iBufferLens = sizeof(iPlayerID) + sizeof(sType) + iLens;
    size_t iHeadLens = Q_INIT_NUMBER;
    const char *pszHead = m_objTcpParser.createHead(iBufferLens, iHeadLens);

    m_objMutex.Lock();
    Q_SockWrite(fd, pszHead, iHeadLens);
    Q_SockWrite(fd, (const char *)&iPlayerID, sizeof(iPlayerID));
    Q_SockWrite(fd, (const char *)&sType, sizeof(sType));
    if (0 != iLens
        && NULL != pszMsg)
    {
        Q_SockWrite(fd, pszMsg, iLens);
    }
    m_objMutex.unLock();
}

bool CDBLoger::Link(void)
{
    try
    {
        m_objLinker.open(m_objUrl);
    }
    catch (CException &e)
    {
        Q_Printf("%s", e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "%s", e.getErrorMsg());

        return false;
    }

    return true;
}

bool CDBLoger::Check(void)
{
    try
    {
        m_objLinker.execDML(Q_FormatStr("select id from %s limit 1", LOG_TABLE).c_str());
    }
    catch(CException &)
    {
        return false;
    }

    return true;
}

bool CDBLoger::createTable(std::string &strName)
{
    try
    {
        m_objLinker.execDML(Q_FormatStr("CREATE TABLE IF NOT EXISTS %s like %s;", 
            strName.c_str(), LOG_TABLE).c_str());
    }
    catch(CException &e)
    {
        Q_Printf("%s", e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "%s", e.getErrorMsg());

        return false;
    }

    return true;
}

void CDBLoger::Write(const char *pszMsg, const size_t iLens)
{
    bool bError = false;
    int64_t iPlayerID = *((int64_t *)pszMsg);
    short sType = *((short *)(pszMsg + sizeof(iPlayerID)));
    const char *pszBuffer = pszMsg + sizeof(sType) + sizeof(iPlayerID);
    size_t iBufLens = iLens - sizeof(sType) - sizeof(iPlayerID);

    //�������
    std::string strNewTable = getTableNam();
    if (strNewTable != m_strTable)
    {
        if (createTable(strNewTable))
        {
            if (NULL != m_pStatement)
            {
                m_pStatement->finalize();
                m_pStatement = NULL;
            }

            m_strTable = strNewTable;
        }
    }

    //ִ��
    try
    {
        if (NULL == m_pStatement)
        {
            m_pStatement = m_objLinker.compileStatement(Q_FormatStr("insert into %s(playerid,logtype, logtime, info) values (?,?,?,?);",
                m_strTable.c_str()).c_str());
        }

        m_pStatement->bindInt64(0, iPlayerID);
        m_pStatement->bindInt(1, sType);
        m_pStatement->bindInt64(2, time(NULL));
        m_pStatement->bindBlob(3, (const unsigned char *)pszBuffer, iBufLens);

        m_pStatement->execDML();
    }
    catch(CException &e)
    {
        Q_Printf("open mysql link error. code %d, messgae %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "open mysql link error. code %d, messgae %s",
            e.getErrorCode(), e.getErrorMsg());

        bError = true;
    }

    if (!bError)
    {
        return;
    }
    if (Check())
    {
        return;
    }

    if (NULL != m_pStatement)
    {
        m_pStatement->finalize();
        m_pStatement = NULL;
    }

    m_objLinker.close();
    Link();
}
