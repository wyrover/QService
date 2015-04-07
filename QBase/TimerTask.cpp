/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 * github:https://github.com/QService/QService
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this std::list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this std::list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "TimerTask.h"
#include "Exception.h"
#include "SockPairEventParam.h"

struct TimerTask
{
    CTask *pTask;
    struct event *m_pEvent;
    SockPairEventParam *pParam;
    unsigned int uiRunCount;
    unsigned int uiCount;
    unsigned int uiMS;    
    TimerTask() : pTask(NULL), m_pEvent(NULL), 
        pParam(NULL), uiRunCount(Q_INIT_NUMBER)
    {
    };
};

CTimerTask::CTimerTask(void)
{
    setTcpParam(&m_lstTask);
    setExitParam(&m_lstTask);
}

int CTimerTask::Append(CTask *pTask, unsigned int uiMS, unsigned int uiCount)
{
    if (NULL == pTask)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return Q_ERROR_NULLPOINTER;
    }
    
    if (uiMS <= 0)
    {
        Q_Printf("%s", "time must big than zero");

        return Q_RTN_FAILE;
    }

    if (RunStatus_Runing != getRunStatus())
    {
        Q_Printf("%s", "event loop not run.");
        Q_SafeDelete(pTask);

        return Q_RTN_FAILE;
    }

    TimerTask stTimerTask;

    stTimerTask.pTask = pTask;
    stTimerTask.uiCount = uiCount;
    stTimerTask.uiMS = uiMS;

    m_objMutex.Lock();
    int iRtn = sendTcpMsg((const char*)(&stTimerTask), sizeof(stTimerTask));
    m_objMutex.unLock();
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "send message error");
        Q_SafeDelete(pTask);

        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

static void time_cb(evutil_socket_t fd, short event, void *arg)
{

    TimerTask *pTimerTask = (TimerTask *)arg;

    pTimerTask->pTask->Run();

    if (Q_INIT_NUMBER == pTimerTask->uiCount)
    {
        return;
    }

    pTimerTask->uiRunCount++;

    if (pTimerTask->uiRunCount >= pTimerTask->uiCount)
    {
        std::list<TimerTask *>::iterator itTask;
        std::list<TimerTask *> *plstUserData = (std::list<TimerTask *> *)(pTimerTask->pParam->pUserDate);
        for (itTask = plstUserData->begin(); plstUserData->end() != itTask; itTask++)
        {
            if ((*itTask)->m_pEvent == pTimerTask->m_pEvent)
            {
                plstUserData->erase(itTask);
                break;
            }
        }

        if (NULL != pTimerTask->m_pEvent)
        {
            event_del(pTimerTask->m_pEvent);
            event_free(pTimerTask->m_pEvent);
        }

        if (NULL != pTimerTask->pTask)
        {
            Q_SafeDelete(pTimerTask->pTask);
        }

        Q_SafeDelete(pTimerTask);
    }
};

void CTimerTask::onTcpRead(SockPairEventParam *pParam)
{
    TimerTask *pTimerTask = NULL;
    timeval tVal;

    while(NULL != (pTimerTask = Q_GetEventValue<TimerTask>(pParam->pEventBuf)))
    {
        pTimerTask->pParam = pParam;
        pTimerTask->m_pEvent = event_new(pParam->pMainBase, 
            -1, EV_PERSIST, time_cb, pTimerTask);
        if (NULL == pTimerTask->m_pEvent)
        {
            Q_Printf("%s", "event_new error");
            Q_SafeDelete(pTimerTask->pTask);
            Q_SafeDelete(pTimerTask);

            continue;
        }
        
        evutil_timerclear(&tVal);
        if (pTimerTask->uiMS >= 1000)
        {
            tVal.tv_sec += pTimerTask->uiMS / 1000;
            tVal.tv_usec += (pTimerTask->uiMS % 1000) * (1000);
        }
        else
        {
            tVal.tv_usec += (pTimerTask->uiMS * 1000);
        }

        if (Q_RTN_OK != event_add(pTimerTask->m_pEvent, &tVal))
        {
            Q_Printf("%s", "event_add error");

            event_free(pTimerTask->m_pEvent);
            Q_SafeDelete(pTimerTask->pTask);
            Q_SafeDelete(pTimerTask);

            continue;
        }

        ((std::list<TimerTask *>*)(pParam->pUserDate))->push_back(pTimerTask);
    }
}

void CTimerTask::onStop(SockPairEventParam *pParam)
{
    std::list<TimerTask *>::iterator itTask;
    std::list<TimerTask *> *plstUserData = (std::list<TimerTask *> *)(pParam->pUserDate);

    for (itTask = plstUserData->begin(); plstUserData->end() != itTask; itTask++)
    {
        if (NULL != (*itTask)->m_pEvent)
        {
            event_del((*itTask)->m_pEvent);
            event_free((*itTask)->m_pEvent);
        }

        if (NULL != (*itTask)->pTask)
        {
            Q_SafeDelete((*itTask)->pTask);
        }

        Q_SafeDelete((*itTask));
    }

    plstUserData->clear();

    setRunStatus(RunStatus_Stopped);
}
