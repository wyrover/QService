
#include "TimerTrigger.h"
#include "Thread.h"
#include "WorkThreadEvent.h"

SINGLETON_INIT(CTimerTrigger)
CTimerTrigger objTrigger;

CTimerTrigger::CTimerTrigger(void) : m_pBase(NULL), m_pExitEvent(NULL), m_pTimerEvent(NULL)
{
    m_pBase = m_objInitBase.getBase();
}

CTimerTrigger::~CTimerTrigger(void)
{
    try
    {
        if (NULL != m_pTimerEvent)
        {
            event_free(m_pTimerEvent);
            m_pTimerEvent = NULL;
        }

        if (NULL != m_pExitEvent)
        {
            event_free(m_pExitEvent);
            m_pExitEvent = NULL;
        }
    }
    catch(...)
    {

    }
}

void CTimerTrigger::exitMonitorCB(evutil_socket_t, short, void *arg)
{
    CTimerTrigger *pTriggerr = CTimerTrigger::getSingletonPtr();
    struct event_base *pBase = (struct event_base *)arg;

    switch(pTriggerr->getRunStatus())
    {
    case RUNSTATUS_STOPPING:
        {
            Q_Printf("ready stop timer trigger thread.");
            pTriggerr->setRunStatus(RUNSTATUS_STOPPED);            
        }
        break;

    case RUNSTATUS_STOPPED:
        {
            Q_Printf("stop timer trigger thread successfully.");
            event_base_loopbreak(pBase);
        }
        break;

    default:
        break;
    }
}

int CTimerTrigger::initExitMonitor(unsigned int uiMS)
{
    timeval tVal;
    evutil_timerclear(&tVal);
    if (uiMS >= 1000)
    {
        tVal.tv_sec = uiMS / 1000;
        tVal.tv_usec = (uiMS % 1000) * (1000);
    }
    else
    {
        tVal.tv_usec = (uiMS * 1000);
    }

    m_pExitEvent = event_new(m_pBase, 
        -1, EV_PERSIST, exitMonitorCB, m_pBase);
    if (NULL == m_pExitEvent)
    {
        Q_Printf("%s", "event_new error");

        return Q_RTN_FAILE;
    }

    if (Q_RTN_OK != event_add(m_pExitEvent, &tVal))
    {
        Q_Printf("%s", "event_add error");

        event_free(m_pExitEvent);
        m_pExitEvent = NULL;

        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

void CTimerTrigger::timerCB(evutil_socket_t, short, void *arg)
{
    char cVal = 1;
    CWorkThreadEvent *pWorker = CWorkThreadEvent::getSingletonPtr();

    if (pWorker->getIsRun())
    {
        pWorker->sendTimerMsg(&cVal, sizeof(cVal));
    }    
}

int CTimerTrigger::setTime(const unsigned int &uiMS)
{
    timeval tVal;
    evutil_timerclear(&tVal);
    if (uiMS >= 1000)
    {
        tVal.tv_sec = uiMS / 1000;
        tVal.tv_usec = (uiMS % 1000) * (1000);
    }
    else
    {
        tVal.tv_usec = (uiMS * 1000);
    }

    m_pTimerEvent = event_new(m_pBase, 
        -1, EV_PERSIST, timerCB, NULL);
    if (NULL == m_pTimerEvent)
    {
        Q_Printf("%s", "event_new error");

        return Q_RTN_FAILE;
    }

    if (Q_RTN_OK != event_add(m_pTimerEvent, &tVal))
    {
        Q_Printf("%s", "event_add error");

        event_free(m_pTimerEvent);
        m_pTimerEvent = NULL;

        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

void CTimerTrigger::Stop(void)
{
    if (getIsRun())
    {
        m_objMutex_Exit.Lock();
        setRunStatus(RUNSTATUS_STOPPING);
        m_objCond_Exit.Wait(&m_objMutex_Exit);    
        m_objMutex_Exit.unLock();
    }
}

int CTimerTrigger::Start(void)
{
    int iRtn = Q_RTN_OK;

    setRunStatus(RUNSTATUS_STARTING);

    iRtn = initExitMonitor(100);
    if (Q_RTN_OK != iRtn)
    {
        setRunStatus(RUNSTATUS_ERROR);

        return iRtn;
    }

    setRunStatus(RUNSTATUS_RUNING);
    iRtn = event_base_dispatch(m_pBase);
    setRunStatus(RUNSTATUS_STOPPED);

    m_objMutex_Exit.Lock();
    m_objCond_Exit.Signal();
    m_objMutex_Exit.unLock();

    Q_Printf("%s", "timer trigger service stopped.");

    return iRtn;
}
