
#ifndef Q_CTASK_H_
#define Q_CTASK_H_

//线程、定时任务基类
class CTask
{
public:
    CTask(void) : m_bDel(true)
    {

    };
    virtual ~CTask()
    {

    };

    void setDel(const bool bDel)
    {
        m_bDel = bDel;
    };
    bool getDel(void)
    {
        return m_bDel;
    };


    /*需要实现的接口*/
    virtual void Run(void) = 0;

private:
    bool m_bDel;
};

#endif//Q_CTASK_H_
