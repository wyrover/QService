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

#include "QFilter.h"

CFilter::CFilter(void) : m_pmapFilter(NULL)
{

}

CFilter::~CFilter(void)
{
    if (NULL != m_pmapFilter)
    {
        try
        {
            freeNode(m_pmapFilter);
        }
        catch(...)
        {
            
        }

        m_pmapFilter = NULL;
    }
}

void CFilter::freeNode(FilterMap *pFilterMap)
{
    std::tr1::unordered_map<char, FilterMap*>::iterator itFilter;

    for (itFilter = pFilterMap->mapFilterMap.begin(); pFilterMap->mapFilterMap.end() != itFilter; 
        itFilter++)
    {
        freeNode(itFilter->second);
    }

    Q_SafeDelete(pFilterMap);
}

void CFilter::Clear(void)
{
    if (NULL != m_pmapFilter)
    {
        freeNode(m_pmapFilter);
        m_pmapFilter = NULL;
    }    
}

/************************************************************************
* Function name:addSensitiveWord
* Description  :添加敏感词
* IN           :pszWord --敏感词、过滤词 uiLens --长度
* OUT          :NONE
* Return       :bool
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
bool CFilter::addSensitiveWord(const char *pszWord, const size_t uiLens)
{
    if (NULL == pszWord
        || 0 == uiLens)
    {
        return false;
    }

    if (NULL == m_pmapFilter)
    {
        m_pmapFilter = new(std::nothrow) FilterMap();
        if (NULL == m_pmapFilter)
        {
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

            return false;
        }
    }

    FilterMap *pSearchMap = m_pmapFilter;
    std::tr1::unordered_map<char, FilterMap*>::iterator itVal;

    //构造字典
    for (size_t i = 0; i < uiLens; i++)
    {
        itVal = pSearchMap->mapFilterMap.find(pszWord[i]);
        if (pSearchMap->mapFilterMap.end() == itVal)
        {
            FilterMap *pstFilterMap = new(std::nothrow) FilterMap();
            if (NULL == pstFilterMap)
            {
                Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

                return false;
            }

            pSearchMap->mapFilterMap[pszWord[i]] = pstFilterMap;
            pSearchMap = pstFilterMap;
        }
        else
        {
            pSearchMap = itVal->second;
        }
    }

    return true;
}

/************************************************************************
* Function name:checkHave
* Description  :检查输入是否包含敏感词
* IN           :pszWord --要检查的字符 uiLens --长度
* OUT          :NONE
* Return       :bool
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
bool CFilter::checkHave(const char *pszWord, const size_t uiLens)
{
    if (NULL == pszWord
        || 0 == uiLens
        || NULL == m_pmapFilter)
    {
        return false;
    }

    FilterMap *pSearchMap = m_pmapFilter;
    std::tr1::unordered_map<char, FilterMap*>::iterator itVal;

    for (size_t i = 0; i < uiLens; i++)
    {
        itVal = pSearchMap->mapFilterMap.find(pszWord[i]);
        if (pSearchMap->mapFilterMap.end() != itVal)
        {
            pSearchMap = itVal->second;
            if (pSearchMap->mapFilterMap.empty())
            {
                return true;
            }

            continue;
        }

        pSearchMap = m_pmapFilter;
        itVal = pSearchMap->mapFilterMap.find(pszWord[i]);
        if (pSearchMap->mapFilterMap.end() != itVal)
        {
            pSearchMap = itVal->second;
            if (pSearchMap->mapFilterMap.empty())
            {
                return true;
            }
        }
    }

    return false;
}

/************************************************************************
* Function name:checkAndModify
* Description  :检查输入是否包含敏感词，如果包含则替换为指定字符
* IN           :pWord --要检查的字符 uiLens --长度 cReplace --替换成的字符
* OUT          :pWord
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
void CFilter::checkAndModify(char *pWord, const size_t uiLens, const char cReplace)
{
    if (NULL == pWord
        || 0 == uiLens
        || NULL == m_pmapFilter)
    {
        return;
    }

    int iBegain = -1;
    FilterMap *pSearchMap = m_pmapFilter;
    std::tr1::unordered_map<char, FilterMap*>::iterator itVal;

    for (size_t i = 0; i < uiLens; i++)
    {
        itVal = pSearchMap->mapFilterMap.find(pWord[i]);
        if (pSearchMap->mapFilterMap.end() != itVal)
        {
            if (-1 == iBegain)
            {
                iBegain = (int)i;
            }

            pSearchMap = itVal->second;
            if (pSearchMap->mapFilterMap.empty())
            {
                memset(pWord + iBegain, cReplace, (size_t)(i + 1 - (size_t)iBegain));
                iBegain = -1;
                pSearchMap = m_pmapFilter;
            }

            continue;
        }

        iBegain = -1;
        pSearchMap = m_pmapFilter;
        itVal = pSearchMap->mapFilterMap.find(pWord[i]);
        if (pSearchMap->mapFilterMap.end() != itVal)
        {
            if (-1 == iBegain)
            {
                iBegain = (int)i;
            }

            pSearchMap = itVal->second;
            if (pSearchMap->mapFilterMap.empty())
            {
                memset(pWord + iBegain, cReplace, (size_t)(i + 1 - (size_t)iBegain));
                iBegain = -1;
                pSearchMap = m_pmapFilter;
            }
        }
    }

    return;
}

const char * CFilter::Filter(const char *pWord, const size_t uiLens)
{
    checkAndModify(const_cast<char*>(pWord), uiLens);

    return pWord;
}
