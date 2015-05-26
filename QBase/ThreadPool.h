
#ifndef Q_THREADPOOL_H_
#define Q_THREADPOOL_H_

#include "Thread.h"

/*线程池类*/
class CThreadPool
{
public:
    /*uiThreadNum 线程池包含的线程数*/
    explicit CThreadPool(const unsigned int uiThreadNum);
    ~CThreadPool(void);

    /*向线程池中增加任务*/
    void Append(CTask *pTask, TaskLevel taskLV = TASKLV_NORMAL, const bool bDel = true);
    /*获取等待队列中任务数量*/
    size_t getTaskNumber(void);
    /*获取线程池大小*/
    size_t getPoolSize(void) const;

private:
    CThreadPool(void);
    int Init(const unsigned int &uiThreadNum);
    void Destroy(void);

private:
    struct ThreadPool *m_pstPool;
};

#endif//Q_THREADPOOL_H_
