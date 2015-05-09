
#ifndef Q_COND_H_
#define Q_COND_H_

#include "Mutex.h"

/*信号量类,失败有异常抛出*/
class CCond
{
public:
    CCond(void);
    ~CCond(void);

    /*等待信号量*/
    void Wait(CQMutex *pMutex);
    /*等待信号量，并指定超时时间，单位豪秒*/
    void Wait(CQMutex *pMutex, const unsigned int uimSec);

    /*激活信号量*/
    void Signal(void);

private:
    int Init(void);
    void Destroy(void);

private:
    Q_Cond_t m_Cond;
};

#endif//Q_COND_H_
