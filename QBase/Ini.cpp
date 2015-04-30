/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 * github:https://github.com/QService/QService
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this std::list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this std::list of conditions and the following disclaimer in the
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

#include "Ini.h"

CIniFile::CIniFile(void)
{

}

CIniFile::CIniFile(const char *pszFile)
{
    assert(NULL != pszFile);
    m_strFile = pszFile;
    readInfo();
}

CIniFile::~CIniFile(void)
{

}

void CIniFile::setFile(const char *pszFile)
{
    assert(NULL != pszFile);

    m_strFile = pszFile;
    readInfo();
}

bool CIniFile::isNote(const std::string &str) const
{    
    if ((std::string("//") == str.substr(0, strlen("//")))
        || (std::string("#") == str.substr(0, strlen("#"))))
    {
        return true;
    }

    return false;
}

void CIniFile::removeNote(std::string &str) const
{
    str = Q_GetFrontOfFlag(str, "//");
    str = Q_GetFrontOfFlag(str, "#");
}

bool CIniFile::isNode(const std::string &str) const
{
    if ((std::string("[") == str.substr(0, strlen("[")))
        && (std::string("]") == str.substr(str.size() - 1, strlen("]")))
        && str.size() > 2)
    {
        return true;
    }

    return false;
}

std::string CIniFile::getNode(const std::string &str) const
{
    std::string strTmp = str.substr(1, str.size() - 2);

    return Q_Trim(strTmp);
}

bool CIniFile::isKey(const std::string &str) const
{
    if (std::string::npos == str.find("="))
    {
        return false;
    }

    return true;
}

std::string CIniFile::getKey(const std::string &str) const
{   
    std::string strTmp = Q_GetFrontOfFlag(str, "=");

    return Q_Trim(strTmp);
}

std::string CIniFile::getVal(const std::string &str) const
{
    std::string::size_type iPos = Q_INIT_NUMBER;

    iPos = str.find_first_of("=");
    if (std::string::npos == iPos)
    {
        return "";
    }

    std::string strTmp = str.substr(iPos + 1, str.size() - 1);

    return Q_Trim(strTmp);
}

void CIniFile::readInfo(void)
{
    m_lstNodeInfo.clear();

    std::fstream inStream(m_strFile.c_str(), std::ios::in);
    if (!inStream.good())
    {
        Q_Printf("open file %s error.", m_strFile.c_str());
        inStream.close();

        return;
    }

    char pBuffer[Q_ONEK];
    std::string strTmp;
    std::string strNode;

    while(inStream.good())
    {
        Q_Zero(pBuffer, sizeof(pBuffer));

        inStream.getline(pBuffer, (std::streamsize)(sizeof(pBuffer) - 1));

        strTmp = std::string(pBuffer);
        strTmp = Q_Trim(strTmp);
        if (strTmp.empty()
            || isNote(strTmp))
        {
            continue;
        }

        //去掉注释
        removeNote(strTmp);
        strTmp = Q_Trim(strTmp);
        if (strTmp.empty())
        {
            continue;
        }

        if (isNode(strTmp))
        {
            strNode = getNode(strTmp);

            continue;
        }

        if (isKey(strTmp))
        {
            setStringValue(strNode.c_str(), getKey(strTmp).c_str(), getVal(strTmp).c_str());
        }
    }

    inStream.close();

    return;
}

const char *CIniFile::getStringValue(const char *pszNode, const char *pszKey, const char *pszDefault)
{
    assert(NULL != pszNode);
    assert(NULL != pszKey);

    std::list<stIniNode>::iterator itNodeInfo;
    std::list<stIniKey>::iterator itKeyInfo;

    for (itNodeInfo = m_lstNodeInfo.begin(); m_lstNodeInfo.end() != itNodeInfo; itNodeInfo++)
    {
        if (std::string(pszNode) == itNodeInfo->strNode)//先找到node
        {
            for (itKeyInfo = itNodeInfo->lstKey.begin(); itNodeInfo->lstKey.end() != itKeyInfo;
                itKeyInfo++)
            {
                if (std::string(pszKey) == itKeyInfo->strKey)//找到key
                {
                    return itKeyInfo->strValue.c_str();
                }
            }
        }
    }

    return pszDefault;
}

int CIniFile::getIntValue(const char *pszNode, const char *pszKey, int iDefault)
{
    const char *pszVal = getStringValue(pszNode, pszKey, NULL);
    if (NULL == pszVal)
    {
        return iDefault;
    }

    if (0 == strlen(pszVal))
    {
        return iDefault;
    }

    return atoi(pszVal);
}

double CIniFile::getFloatValue(const char *pszNode, const char *pszKey, double dDefault)
{
    const char *pszVal = getStringValue(pszNode, pszKey, NULL);
    if (NULL == pszVal)
    {
        return dDefault;
    }

    if (0 == strlen(pszVal))
    {
        return dDefault;
    }

    return atof(pszVal);
}

void CIniFile::setStringValue(const char *pszNode, const char *pszKey, 
    const char *pszValue)
{
    assert(NULL != pszNode);
    assert(NULL != pszKey);
    assert(NULL != pszValue);

    std::list<stIniNode>::iterator itNodeInfo;
    std::list<stIniKey>::iterator itKeyInfo;

    for (itNodeInfo = m_lstNodeInfo.begin(); m_lstNodeInfo.end() != itNodeInfo; itNodeInfo++)
    {
        if (std::string(pszNode) == itNodeInfo->strNode)//先找到node
        {
            for (itKeyInfo = itNodeInfo->lstKey.begin(); itNodeInfo->lstKey.end() != itKeyInfo;
                itKeyInfo++)
            {
                if (std::string(pszKey) == itKeyInfo->strKey)//找到key
                {
                    itKeyInfo->strValue = pszValue;

                    return;
                }
            }

            //没有key则加入
            stIniKey stKey;
            stKey.strKey = pszKey;
            stKey.strValue = pszValue;
            itNodeInfo->lstKey.push_back(stKey);

            return;
        }
    }

    //没有Node则加入
    stIniNode stNode;
    stIniKey stKey;
    stKey.strKey = pszKey;
    stKey.strValue = pszValue;

    stNode.strNode = pszNode;
    stNode.lstKey.push_back(stKey);

    m_lstNodeInfo.push_back(stNode);

    return;
}

void CIniFile::setIntValue(const char *pszNode, const char *pszKey, 
    int iValue)
{
    setStringValue(pszNode, pszKey, Q_ToString(iValue).c_str());
}

void CIniFile::setFloatValue(const char *pszNode, const char *pszKey, 
    double dValue)
{
    setStringValue(pszNode, pszKey, Q_ToString(dValue).c_str());
}

void CIniFile::delKey(const char *pszNode, const char *pszKey)
{
    assert(NULL != pszNode);
    assert(NULL != pszKey);

    std::list<stIniNode>::iterator itNodeInfo;
    std::list<stIniKey>::iterator itKeyInfo;

    for (itNodeInfo = m_lstNodeInfo.begin(); m_lstNodeInfo.end() != itNodeInfo; itNodeInfo++)
    {
        if (std::string(pszNode) == itNodeInfo->strNode)//先找到node
        {
            for (itKeyInfo = itNodeInfo->lstKey.begin(); itNodeInfo->lstKey.end() != itKeyInfo;
                itKeyInfo++)
            {
                if (std::string(pszKey) == itKeyInfo->strKey)//找到key
                {
                    itNodeInfo->lstKey.erase(itKeyInfo);

                    return;
                }
            }
        }
    }
}

void CIniFile::delNode(const char *pszNode)
{
    assert(NULL != pszNode);

    std::list<stIniNode>::iterator itNodeInfo;

    for (itNodeInfo = m_lstNodeInfo.begin(); m_lstNodeInfo.end() != itNodeInfo; itNodeInfo++)
    {
        if (std::string(pszNode) == itNodeInfo->strNode)//先找到node
        {
            m_lstNodeInfo.erase(itNodeInfo);

            return;
        }
    }
}

void CIniFile::saveTo(const char *pszFile)
{
    assert(NULL != pszFile);

    FILE *pFile = NULL;
    std::string strTmp;
    std::list<stIniNode>::iterator itNodeInfo;
    std::list<stIniKey>::iterator itKeyInfo;    

    pFile = fopen(pszFile, "w");
    if (NULL == pFile)
    {
        return;
    }

    for (itNodeInfo = m_lstNodeInfo.begin(); m_lstNodeInfo.end() != itNodeInfo; itNodeInfo++)
    {
        strTmp = std::string("[") + itNodeInfo->strNode + std::string("]\n");
        fwrite(strTmp.c_str(), 1, strTmp.size(), pFile);

        for (itKeyInfo = itNodeInfo->lstKey.begin(); itNodeInfo->lstKey.end() != itKeyInfo;
            itKeyInfo++)
        {
            strTmp = itKeyInfo->strKey + std::string(" = ") + itKeyInfo->strValue + std::string("\n");
            fwrite(strTmp.c_str(), 1, strTmp.size(), pFile);
        }
    }

    fclose(pFile);

    return;
}

void CIniFile::Save(void)
{
    saveTo(m_strFile.c_str());
}
