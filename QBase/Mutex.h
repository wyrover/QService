
#ifndef Q_MUTEX_H_
#define Q_MUTEX_H_

#include "Macros.h"

/*互斥锁类,失败有异常抛出*/
class CQMutex
{
public:
    CQMutex(void);
    ~CQMutex(void);
    
    /*锁定*/
    void Lock(void);
    /*解锁*/
    void unLock(void);

    /*返回Q_Mutex_t,主要信号量时候要用*/
    Q_Mutex_t *getMutex(void);

private:
    int Init(void);
    void Destroy(void);

private:
    Q_Mutex_t m_Mutex;
};

#endif//Q_MUTEX_H_
