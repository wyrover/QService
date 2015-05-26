
#ifndef Q_OBJPOOL_H_
#define Q_OBJPOOL_H_

#include "Macros.h"

//ºÚµ•∂‘œÛ≥ÿ
template <typename T>
class CObjPool
{
public:
    CObjPool(const unsigned int uiInitNum, const char *pszPoolName = "");
    ~CObjPool(void);

    T* Malloc(void);
    void Free(T *pObj);

    size_t getUsedSize(void);
    size_t getFreeSize(void);

private:
    CObjPool(void);
    void Init(const unsigned int &uiInitNum);    
    void Clear(void);

private:
    unsigned int m_uiCount;
    std::list<T*> m_lstUsed;
    std::queue<T*> m_quFree;
    std::string m_strName;
};

template <typename T>
CObjPool<T>::CObjPool(const unsigned int uiInitNum, const char *pszPoolName) : 
    m_uiCount(Q_INIT_NUMBER), m_strName(pszPoolName)
{
    Init(uiInitNum);
}

template <typename T>
CObjPool<T>::~CObjPool(void)
{
    if (Q_INIT_NUMBER != m_uiCount)
    {
        Q_Printf("warning: object pool %s maybe some object not free.", m_strName.c_str());
    }

    Clear();
}

template <typename T>
void CObjPool<T>::Init(const unsigned int &uiInitNum)
{
    if (Q_INIT_NUMBER == uiInitNum)
    {
        return;
    }

    for (unsigned int ui = 0; ui < uiInitNum; ui++)
    {
        m_quFree.push(new T);
    }
}

template <typename T>
void CObjPool<T>::Clear(void)
{
    typename std::list<T*>::iterator itList;
    for (itList = m_lstUsed.begin(); m_lstUsed.end() != itList; itList++)
    {
        Q_SafeDelete(*itList);
    }
    m_lstUsed.clear();

    T *pTmp = NULL;
    while(!m_quFree.empty())
    {
        pTmp = m_quFree.front();
        m_quFree.pop();
        Q_SafeDelete(pTmp);
    }
}

template <typename T>
T* CObjPool<T>::Malloc(void)
{
    T *pTmp = NULL;

    if (m_quFree.empty())
    {
        pTmp = new T;
    }
    else
    {
        pTmp = m_quFree.front();
        m_quFree.pop();
    }

    m_lstUsed.push_back(pTmp);
    m_uiCount++;

    return pTmp;
}

template <typename T>
void CObjPool<T>::Free(T *pObj)
{
    if (NULL == pObj)
    {
        return;
    }

    typename std::list<T*>::iterator itList;
    for (itList = m_lstUsed.begin(); m_lstUsed.end() != itList; itList++)
    {
        if (*itList == pObj)
        {
            m_lstUsed.erase(itList);
            m_quFree.push(pObj);
            m_uiCount--;

            break;
        }
    }
}

template <typename T>
size_t CObjPool<T>::getUsedSize(void)
{
    return m_lstUsed.size();
}

template <typename T>
size_t CObjPool<T>::getFreeSize(void)
{
    return m_quFree.size();
}

#endif//Q_OBJPOOL_H_
