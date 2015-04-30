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

#ifndef Q_QSTRING_H_
#define Q_QSTRING_H_

#include "Macros.h"

/*转大写*/
void Q_Convert2Upper(char *pszWord);
std::string &Q_Convert2Upper(std::string &strWord);
/*转小写*/
void Q_Convert2Lower(char *pszWord);
std::string &Q_Convert2Lower(std::string &strWord);
/*将字符串的iBg和iEnd之间的字符反序(iBg, iEnd从0开始)*/
void Q_ReverseStr(char *pszWord, const size_t iBg, const size_t iEnd);
std::string &Q_ReverseStr(std::string &strWord, const size_t iBg, const size_t iEnd);
/*将符串的最后n个字符移到字符串前面*/
void Q_MoveN2Before(char *pszWord, const size_t n);
std::string &Q_MoveN2Before(std::string &strWord, const size_t n);

/*wchar_t 转 char*/
int Q_WideCharToMultiByte(const wchar_t *pszSrc, char *pszDest);
/*char 转 wchar_t*/
int Q_MultiByteToWideChar(const char *pszSrc, wchar_t *pszDest);

/*转字符串*/
template<typename T>
std::string Q_ToString(T number)
{
    std::stringstream objStream;

    objStream << number;

    return objStream.str();
};

/*转数字*/
template<typename T>
T Q_ToNumber(const char *pszNumber)
{
    T  iValue = Q_INIT_NUMBER;
    std::stringstream objStream;

    objStream << pszNumber;
    objStream >> iValue;

    return iValue;
};

/*去掉字符串中所有空格,\n,\r,\t*/
std::string &Q_TrimAll(std::string &strSource);
/*去掉左边字符串中所有空格,\n,\r,\t*/
std::string &Q_TrimLeft(std::string &strSource);
/*去掉右边字符串中所有空格,\n,\r,\t*/
std::string &Q_TrimRight(std::string &strSource);
/*去掉两边字符串中所有空格,\n,\r,\t*/
std::string &Q_Trim(std::string &strSource);

/*字符串拆分*/
void Q_Split(const std::string &strSource, const char *pszFlag,
    std::list<std::string> &lstRst);

/*取得标记第一次出现之前的字符串*/
std::string Q_GetFrontOfFlag(const std::string &strSource, const char *pszFlag);
/*取得标记最后一次出现之后的字符串*/
std::string Q_GetLastOfFlag(const std::string &strSource, const char *pszFlag);

/*字符串替换,简单将原字符串中的字符替换为指定字符*/
std::string &Q_Replace(std::string &strSource, const char *pszFlag, 
    const char *pszReFlag);
/*字符串替换,替换原字符串中字符成指定字符，并且要替换的字符在原字符中不出现*/
std::string &Q_Replace_Depth(std::string &strSource, const char *pszFlag, 
    const char *pszReFlag);

/*字符格式化*/
std::string Q_FormatVa(const char *pcFormat, va_list args);
/*字符格式化*/
std::string Q_FormatStr(const char *pcFormat, ...);

#endif//Q_QSTRING_H_
