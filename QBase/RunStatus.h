
#ifndef Q_RUNSTATUS_H_
#define Q_RUNSTATUS_H_

#include "Thread.h"

//运行状态
class CRunStatus
{
public:
    CRunStatus(void) : m_emStatus(RUNSTATUS_UNKNOWN)
    {

    };
    virtual ~CRunStatus(void)
    {
    };

    //等待完成
    bool waitForStarted(void) const
    {
        while(true)
        {
            if (getIsRun())
            {
                return true;
            }

            if (getError())
            {
                return false;
            }

            Q_Sleep(10);
        }
    };
    //状态设置、获取
    void setRunStatus(RunStatus emStatus)
    {
        m_emStatus = emStatus;
    };
    RunStatus getRunStatus(void) const
    {
        return m_emStatus;
    };

    //是否发生错误
    bool getError(void) const
    {
        return ((RUNSTATUS_ERROR == getRunStatus()) ? true : false);
    };
    //是否在运行
    bool getIsRun(void) const
    {
        return ((RUNSTATUS_RUNING == getRunStatus()) ? true : false);
    };

private:
    RunStatus m_emStatus;
};

#endif//Q_RUNSTATUS_H_
