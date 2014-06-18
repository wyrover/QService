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

#ifndef Q_TABLEFILE_H_
#define Q_TABLEFILE_H_

#include "QString.h"
/*
读取格式为：
姓名 年龄 住址
小明 20   龙泉
....
的文本文件
注释为#或者//
*/
class CTableFile
{
public:
    CTableFile(void);
    ~CTableFile(void){};

    //文件名
    void setFile(const char *pszFile);
    const char *getFile(void);
    //拆分标志
    void setSplitFlag(const char *pszSplitFlag);

    //解析
    int Parse(void);
    //是否还有未读行
    bool eof(void);
    //下一行
    void nextRow(void);
    //返回到第一行
    void reSet(void);

    /*读取值*/
    const char *getStringValue(const char *pszName, const char *pszDefault = "");
    int getIntValue(const char *pszName, const int iDefault = 0);
    double getFloatValue(const char *pszName, const double dDefault = 0.0);

    /*返回头*/
    const std::map<int, std::string> *getHead(void)
    {
        return &m_mapTableHead;
    };

private:
    void getValue(std::string &strValue, std::map<int, std::string> &mapTableHead);
    bool getHead(std::string &strHead, std::map<int, std::string> &mapTableHead);
    bool checkHead(std::list<std::string> &lstTableHead);
    bool parseHead(std::list<std::string> &lstTableHead, std::map<int, std::string> &mapTableHead);
    void checkEof(void);
    bool checkNote(std::string &strMsg);

private:
    bool m_bEof;
    std::string m_strFile;
    std::string m_strSplitFlag;
    std::map<int, std::string> m_mapTableHead;
    std::list<std::map<std::string, std::string> >::iterator m_itNowRow;
    std::list<std::map<std::string, std::string> > m_lstAllValue;
};

#endif//Q_TABLEFILE_H_
