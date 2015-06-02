
#ifndef Q_FILTER_H_
#define Q_FILTER_H_

#include "Macros.h"

/*
字符串过滤
*/
class CFilter
{
public:
    CFilter(void);
    ~CFilter(void);

    /*添加敏感词*/
    bool addFilterWord(const char *pszWord, const unsigned int uiLens); 
    /*检查是否包含敏感词*/
    bool checkHave(const char *pszWord, const unsigned int uiLens);
    /*检查敏感词并替换*/    
    const char *Filter(const char *pWord, const unsigned int uiLens);
    /*清理*/
    void Clear(void);

private:
    struct FilterMap
    {
        std::tr1::unordered_map<char, FilterMap*> mapFilterMap;
    };

    FilterMap *m_pmapFilter;

private:
    void freeNode(FilterMap *pFilterMap);
    void checkAndModify(char *pWord, const unsigned int &uiLens, const char cReplace = '*');
};

#endif//Q_FILTER_H_
