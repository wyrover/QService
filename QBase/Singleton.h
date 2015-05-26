
#ifndef Q_SINGLETON_H_
#define Q_SINGLETON_H_

#include "Macros.h"

//¼òµ¥µ¥Àý
template <typename T>
class CSingleton
{
public:
    CSingleton(void)
    {
        assert(NULL == m_Singleton);
        m_Singleton = static_cast< T* >(this);
    };
    virtual ~CSingleton(void)
    {
        assert(NULL != m_Singleton);
        m_Singleton = NULL;
    };

    static T& getSingleton(void)
    {
        assert(NULL != m_Singleton);
        return (*m_Singleton);
    };
    static T* getSingletonPtr(void)
    {
        return m_Singleton;
    };

private:
    static T *m_Singleton;
};

#define SINGLETON_INIT(T)							\
    template <>	 T * CSingleton<T>::m_Singleton = NULL;	\

#endif//Q_SINGLETON_H_
