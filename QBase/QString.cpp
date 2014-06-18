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

#include "QString.h"

/*************************************************
* Function name:Q_Convert2Capital
* Description  :转大写
* IN           :pszWord
* OUT          :pszWord
* Return       :None
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
void Q_Convert2Upper(char *pszWord)
{
    if (NULL == pszWord)
    {
        return;
    }

    int iLens = strlen(pszWord);

    for (int i = 0; i < iLens; i++)
    {
        if (*(pszWord + i) >= 'a' && *(pszWord + i) <= 'z')
        {
            *(pszWord+i) -= ('a' - 'A');
        }
    }

    return;
}

/*************************************************
* Function name:Q_Convert2Capital
* Description  :转大写
* IN           :strWord
* OUT          :strWord
* Return       :string
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
std::string &Q_Convert2Upper(std::string &strWord)
{
    transform(strWord.begin(), strWord.end(), strWord.begin(), ::toupper);  

    return strWord;
}

/*************************************************
* Function name:Q_Convert2Lower
* Description  :转小写
* IN           :pszWord
* OUT          :pszWord
* Return       :None
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
void Q_Convert2Lower(char *pszWord)
{
    if (NULL == pszWord)
    {
        return;
    }

    int iLens = strlen(pszWord);

    for (int i = 0; i < iLens; i++)
    {
        if (*(pszWord + i) >= 'A' && *(pszWord + i) <= 'Z')
        {
            *(pszWord+i) += ('a' - 'A');
        }
    }

    return;
}

/*************************************************
* Function name:Q_Convert2Lower
* Description  :转小写
* IN           :strWord
* OUT          :strWord
* Return       :string
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
std::string &Q_Convert2Lower(std::string &strWord)
{
    transform(strWord.begin(), strWord.end(), strWord.begin(), ::tolower);  

    return strWord;
}

/*************************************************
* Function name:Q_ReverseStr
* Description  :将字符串的iBg和iEnd之间的字符反序(iBg, iEnd从0开始)
* IN           :pszWord iBg  iEnd
* OUT          :pszWord
* Return       :None
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
void Q_ReverseStr(char *pszWord, const size_t iBg, const size_t iEnd)
{
    if (NULL == pszWord)
    {
        return;
    }

    if (1 >= strlen(pszWord))
    {
        return;
    }

    int iBgTmp = Q_INIT_NUMBER;
    int iEndTmp = Q_INIT_NUMBER;
    size_t iLens = strlen(pszWord);

    iBgTmp = (iBg >= iLens ? (iLens - 1) : iBg);
    iEndTmp = (iEnd >= iLens ? (iLens - 1) : iEnd);

    if((iBgTmp >= iEndTmp)) 
    {
        return;
    }

    while(iBgTmp < iEndTmp)
    { 
        *(pszWord + iBgTmp) = *(pszWord + iBgTmp) + *(pszWord + iEndTmp);
        *(pszWord + iEndTmp) = *(pszWord + iBgTmp) - *(pszWord + iEndTmp);
        *(pszWord + iBgTmp) = *(pszWord + iBgTmp) - *(pszWord + iEndTmp);

        iEndTmp--;
        iBgTmp++;
    } 

    return;
}

std::string &Q_ReverseStr(std::string &strWord, const size_t iBg, const size_t iEnd)
{
    if (1 >= strWord.size())
    {
        return strWord;
    }

    int iBgTmp = Q_INIT_NUMBER;
    int iEndTmp = Q_INIT_NUMBER;
    size_t iLens = strWord.size();
    std::string::iterator itStr;

    iBgTmp = (iBg >= iLens ? (iLens - 1) : iBg);
    iEndTmp = (iEnd >= iLens ? (iLens - 1) : iEnd);

    if((iBgTmp >= iEndTmp)) 
    {
        return strWord;
    }

    itStr = strWord.begin();

    while(iBgTmp < iEndTmp)
    { 
        *(itStr + iBgTmp) = *(itStr + iBgTmp) + *(itStr + iEndTmp);
        *(itStr + iEndTmp) = *(itStr + iBgTmp) - *(itStr + iEndTmp);
        *(itStr + iBgTmp) = *(itStr + iBgTmp) - *(itStr + iEndTmp);

        iEndTmp--;
        iBgTmp++;
    } 

    return strWord;
}

/*************************************************
* Function name:Q_MoveN2Before
* Description  :将符串的最后n个字符移到字符串前面
* IN           :pszWord n
* OUT          :pszWord
* Return       :None
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
void Q_MoveN2Before(char *pszWord, const size_t n)
{
    if (NULL == pszWord)
    {
        return;
    }

    size_t iLens = strlen(pszWord);
    if (n >= iLens
        || n <= 0
        || 0 == iLens)
    {
        return;
    }

    Q_ReverseStr(pszWord, 0, (iLens - n - 1)); 
    Q_ReverseStr(pszWord, (iLens - n), (iLens - 1)); 
    Q_ReverseStr(pszWord, 0, (iLens - 1)); 

    return;
}

std::string &Q_MoveN2Before(std::string &strWord, const size_t n)
{
    size_t iLens = strWord.size();

    if (n >= iLens
        || n <= 0
        || 0 == iLens)
    {
        return strWord;
    }

    (void)Q_ReverseStr(strWord, 0, (iLens - n - 1)); 
    (void)Q_ReverseStr(strWord, (iLens - n), (iLens - 1)); 
    (void)Q_ReverseStr(strWord, 0, (iLens - 1)); 

    return strWord;
}

size_t Q_StrHash(const char *pszValues)
{
    register size_t uiHash = Q_INIT_NUMBER;
    register size_t uiTmp = Q_INIT_NUMBER;

    while ((uiTmp = (size_t)(*pszValues++)))
    {
        uiHash = uiHash * 131 + uiTmp;
    }

    return uiHash;
}

/*************************************************
* Function name:Q_WideCharToMultiByte
* Description  :wchar_t 转 char
* IN           :pszSrc --wchar_t
* OUT          :pszDest --char
* Return       :Q_Rtn_Ok 成功 其他 失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
int Q_WideCharToMultiByte(const wchar_t *pszSrc, char *pszDest)
{
    if ((NULL == pszSrc)
        || (NULL == pszDest))
    {
        return Q_ERROR_NULLPOINTER;
    }

#ifdef Q_OS_WIN32
    int iNum = Q_INIT_NUMBER;

    iNum = WideCharToMultiByte(CP_OEMCP, NULL, pszSrc, -1, NULL, 0, NULL, FALSE);
    if (iNum <= Q_INIT_NUMBER)
    {
        return Q_RTN_FAILE;
    }

    iNum = WideCharToMultiByte(CP_OEMCP, NULL, pszSrc, -1, pszDest, iNum, NULL, FALSE);
    if (Q_INIT_NUMBER == iNum)
    {
        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
#else
    size_t uiLen = wcslen(pszSrc);
    if ((size_t)Q_RTN_FAILE == wcstombs(pszDest, pszSrc, uiLen))
    {
        return Q_RTN_FAILE;
    }

    pszDest[uiLen] = 0;

    return Q_RTN_OK;
#endif
}

/*************************************************
* Function name:Q_MultiByteToWideChar
* Description  :char 转 wchar_t
* IN           :pszSrc --char
* OUT          :pszDest --wchar_t
* Return       :Q_Rtn_Ok 成功 其他 失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
int Q_MultiByteToWideChar(const char *pszSrc, wchar_t *pszDest)
{
    if ((NULL == pszSrc)
        || (NULL == pszDest))
    {
        return Q_ERROR_NULLPOINTER;
    }

#ifdef Q_OS_WIN32
    int iNum = Q_INIT_NUMBER;

    iNum = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, NULL, 0);
    if (iNum <= Q_INIT_NUMBER)
    {
        return Q_RTN_FAILE;
    }

    iNum = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pszDest, iNum);
    if (Q_INIT_NUMBER == iNum)
    {
        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
#else
    size_t uiLen = strlen(pszSrc);
    if ((size_t)Q_RTN_FAILE == mbstowcs(pszDest, pszSrc, uiLen))
    {
        return Q_RTN_FAILE;
    }

    pszDest[uiLen] = 0;

    return Q_RTN_OK;
#endif
}

/*************************************************
* Function name:Q_TrimAll
* Description  :去掉字符串中所有空格,\n,\r,\t
* IN           :strSource --源字符串
* OUT          :None
* Return       :string
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string &Q_TrimAll(std::string &strSource)
{
    if (strSource.empty())
    {
        return strSource;
    }

    std::string::iterator itSrc;
    for (itSrc = strSource.begin(); strSource.end() != itSrc;)
    {
        if((' ' == *itSrc)
            || ('\n' == *itSrc)
            || ('\r' == *itSrc)
            || ('\t' == *itSrc))
        {
            itSrc = strSource.erase(itSrc);
            continue;
        }

        itSrc++;
    }

    return strSource;
}

/*************************************************
* Function name:Q_TrimLeft
* Description  :去掉左边字符串中所有空格,\n,\r
* IN           :strSource --源字符串 cFlag
* OUT          :None
* Return       :string
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string &Q_TrimLeft(std::string &strSource)
{
    if (strSource.empty())
    {
        return strSource;
    }

    std::string::iterator itSrc;
    for (itSrc = strSource.begin(); strSource.end() != itSrc;)
    {
        if((' ' == *itSrc)
            || ('\n' == *itSrc)
            || ('\r' == *itSrc))
        {
            itSrc = strSource.erase(itSrc);
            continue;
        }

        break;
    }

    return strSource;
}

/*************************************************
* Function name:Q_TrimRight
* Description  :去掉右边字符串中所有空格,\n,\r
* IN           :strSource --源字符串 cFlag
* OUT          :None
* Return       :string
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string &Q_TrimRight(std::string &strSource)
{
    if (strSource.empty())
    {
        return strSource;
    }

    size_t iFlag = 0;
    size_t iSize = 0;
    std::string::reverse_iterator reitSrc;

    iSize = strSource.size();

    for (reitSrc = strSource.rbegin(); strSource.rend() != reitSrc; reitSrc++, iFlag++)
    {
        if((' ' != *reitSrc)
            && ('\n' != *reitSrc)
            && ('\r' != *reitSrc))
        {
            break;
        }
    }

    if (0 == iFlag)
    {
        return strSource;
    }

    if (iSize == iFlag)
    {
        strSource.clear();

        return strSource;
    }

    std::string::iterator itSrc;
    itSrc = (strSource.begin() + (iSize - iFlag));
    while(strSource.end() != itSrc)
    {
        itSrc = strSource.erase(itSrc);
    }

    return strSource;
}

/*************************************************
* Function name:Q_Trim
* Description  :去掉两边字符串中所有空格,\n,\r
* IN           :strSource --源字符串
* OUT          :None
* Return       :string
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string &Q_Trim(std::string &strSource)
{
    return Q_TrimLeft(Q_TrimRight(strSource));
}

/*************************************************
* Function name:Q_Split
* Description  :字符串拆分
* IN           :strSource --待拆分的字符串 strFlag --拆分标志
* OUT          :lstRst --拆分后的字符串链表
* Return       :None
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
void Q_Split(const std::string &strSource, const std::string &strFlag,
    std::list<std::string> &lstRst)
{
    lstRst.clear();

    if(strSource.empty())
    {
        return;
    }

    if(strFlag.empty())
    {
        return;
    }

    std::string::size_type loc;
    loc = strSource.find(strFlag.c_str(), 0);
    if(std::string::npos == loc)
    {
        lstRst.push_back(strSource);

        return;
    }

    std::string strTmp;
    std::string strRst;

    strTmp = strSource;

    while(std::string::npos != loc)
    {
        strRst.clear();
        strRst = strTmp.substr(0, loc);
        lstRst.push_back(strRst);
        strTmp = strTmp.substr(loc + strFlag.size());

        loc = strTmp.find(strFlag.c_str(), 0);
        if(std::string::npos == loc)
        {
            strRst = strTmp;
            lstRst.push_back(strRst);
        }
    }

    return;
}

std::string Q_GetFrontOfFlag(const std::string &strSource, const char *pszFlag)
{
    std::string::size_type iPos = Q_INIT_NUMBER;

    iPos = strSource.find_first_of(pszFlag);
    if (std::string::npos == iPos)
    {
        return strSource;
    }

    return strSource.substr(0, iPos);
}

std::string Q_GetLastOfFlag(const std::string &strSource, const char *pszFlag)
{
    std::string::size_type iPos = Q_INIT_NUMBER;

    iPos = strSource.find_last_of(pszFlag);
    if (std::string::npos == iPos)
    {
        return strSource;
    }

    return strSource.substr(iPos + strlen(pszFlag), strSource.size());
}

/*************************************************
* Function name:Q_Replace
* Description  :字符串替换,简单将原字符串中的字符替换为指定字符
* IN           :strSource --原字符串 strFlag --要替换的字符串 strReFlag --替换为的
* OUT          :None
* Return       :替换后的字符串 
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string &Q_Replace(std::string &strSource, const std::string &strFlag, 
    const std::string &strReFlag)
{
    for(std::string::size_type pos = 0; std::string::npos != pos; pos += strReFlag.size())   
    {
        pos = strSource.find(strFlag, pos);
        if(std::string::npos != pos)
        {
            strSource.replace(pos, strFlag.length(), strReFlag); 
        }
        else
        {
            break;
        }
    }

    return strSource;
}

/*************************************************
* Function name:Q_Replace_Depth
* Description  :字符串替换,替换原字符串中字符成指定字符，并且要替换的字符在原字符中不出现
* IN           :strSource --原字符串 strFlag --要替换的字符串 strReFlag --替换为的
* OUT          :None
* Return       :替换后的字符串 
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
std::string &Q_Replace_Depth(std::string &strSource, const std::string &strFlag, 
    const std::string &strReFlag)
{
    std::string::size_type pos = 0;

    while(true)
    {
        pos = strSource.find(strFlag);
        if(std::string::npos != pos) 
        {
            strSource.replace(pos, strFlag.size(), strReFlag);
        }
        else
        {
            break;
        }
    }

    return strSource;
}

std::string Q_FormatVa(const char *pcFormat, va_list args)
{
    if (NULL == pcFormat)
    {
        return "";
    }
    int iNum = Q_INIT_NUMBER;
    unsigned int uiSize = Q_ONEK;
    char *pcBuff = NULL;
    std::string strRtn;

    pcBuff = new(std::nothrow) char[uiSize];
    if (NULL == pcBuff)
    {
        return strRtn;
    }
    while(true)
    {        Q_Zero(pcBuff, uiSize);
        iNum = vsnprintf(pcBuff, uiSize, pcFormat, args);
        if ((iNum > -1)
            && (iNum < (int)uiSize))
        {
            strRtn = pcBuff;
            Q_SafeDelete(pcBuff);

            return strRtn;
        }
        //分配更大空间
        uiSize = (iNum > -1) ? (int)(iNum + 1) : uiSize*2;
        Q_SafeDelete(pcBuff);
        pcBuff = new(std::nothrow) char[uiSize];
        if (NULL == pcBuff)
        {
            return "";
        }
    }
    Q_SafeDelete(pcBuff);

    return "";
}

std::string Q_FormatStr(const char *pcFormat, ...)
{
    if (NULL == pcFormat)
    {
        return "";
    }

    va_list va;
    std::string strVa;

    va_start(va, pcFormat);
    strVa = Q_FormatVa(pcFormat, va);
    va_end(va);

    return strVa;
}
