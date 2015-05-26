
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
bool CFilter::addFilterWord(const char *pszWord, const unsigned int uiLens)
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
    for (unsigned int i = 0; i < uiLens; i++)
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
bool CFilter::checkHave(const char *pszWord, const unsigned int uiLens)
{
    if (NULL == pszWord
        || 0 == uiLens
        || NULL == m_pmapFilter)
    {
        return false;
    }

    FilterMap *pSearchMap = m_pmapFilter;
    std::tr1::unordered_map<char, FilterMap*>::iterator itVal;

    for (unsigned int i = 0; i < uiLens; i++)
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
void CFilter::checkAndModify(char *pWord, const unsigned int &uiLens, const char cReplace)
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

    for (unsigned int i = 0; i < uiLens; i++)
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

const char * CFilter::Filter(const char *pWord, const unsigned int uiLens)
{
    checkAndModify(const_cast<char*>(pWord), uiLens);

    return pWord;
}
