
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

    /*向线程池中增加任务,任务指针在执行完成后自动删除*/
    void Append(CTask *pTask, TaskLevel taskLV = Q_ThreadLV_Normal);
    /*获取等待队列中任务数量*/
    unsigned int getTaskNumber(void);
    /*获取线程池大小*/
    unsigned int getPoolSize(void) const;

private:
    CThreadPool(void);
    int Init(const unsigned int &uiThreadNum);
    void Destroy(void);

private:
    struct ThreadPool *m_pstPool;
};

#endif//Q_THREADPOOL_H_
