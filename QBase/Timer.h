
#ifndef Q_TIMER_H_
#define Q_TIMER_H_

#include "Macros.h"

/*
计时器
*/
class CTimer
{
public:
    CTimer(void);
    ~CTimer(void){};

    /*重置计时器*/
    void reStart(void);
    /*返回流逝的时间(ms)*/
    double Elapsed(void);

private:
#ifdef Q_OS_WIN32
    bool m_bUseFrequency;    
    LARGE_INTEGER m_StartCount;//记录开始时间     
    LARGE_INTEGER m_EndCount;//记录结束时间     
    LARGE_INTEGER m_Freq;//本机CPU时钟频率
    struct Q_TIMEB m_BgTime;
    struct Q_TIMEB m_EndTime;
#else
    struct timeval m_stStart;  
    struct timeval m_stEnd;
#endif    
};

#endif//Q_TIMER_H_
