
#ifndef Q_CTASK_H_
#define Q_CTASK_H_

//线程、定时任务基类
class CTask
{
public:
    CTask(void){};
    virtual ~CTask(){};

    /*需要实现的接口*/
    virtual void Run(void) = 0;
};

#endif//Q_CTASK_H_
