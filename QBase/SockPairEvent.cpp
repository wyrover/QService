/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 * github:https://github.com/QService/QService
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
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

#include "SockPairEvent.h"
#include "Thread.h"
#include "Exception.h"
#include "SysLog.h"

CSockPairEvent::CSockPairEvent(void) : m_bRunOnStop(false), m_emStatus(RunStatus_Unknown), 
    m_pBase(NULL), m_pMainBev(NULL),m_pAssistBev(NULL), m_pExitEvent(NULL),
    m_objMainSockPair(), m_objAssistSockPair(), m_objMainBuffer(), m_objAssistBuffer(),
    m_Mutex(), m_Cond()
{
    m_pBase = event_base_new();
    if (NULL == m_pBase)
    {
        Q_Printf("%s", "event_base_new error.");
        setRunStatus(RunStatus_Error);
    }
}

CSockPairEvent::~CSockPairEvent(void)
{
    try
    {
        freeAll();
    }
    catch(...)
    {

    }    
}

void CSockPairEvent::freeAll(void)
{
    if (NULL != m_pMainBev)
    {
        bufferevent_free(m_pMainBev);
        m_pMainBev = NULL;
    }

    if (NULL != m_pAssistBev)
    {
        bufferevent_free(m_pAssistBev);
        m_pAssistBev = NULL;
    }

    if (NULL != m_pExitEvent)
    {
        event_free(m_pExitEvent);
        m_pExitEvent = NULL;
    }

    if (NULL != m_pBase)
    {
        event_base_free(m_pBase);
        m_pBase = NULL;
    }
}

int CSockPairEvent::initExitMonitor(unsigned int uiMS)
{
    timeval tVal;
    evutil_timerclear(&tVal);
    if (uiMS >= 1000)
    {
        tVal.tv_sec = (uiMS / 1000);
        tVal.tv_usec = ((uiMS % 1000) * (1000));
    }
    else
    {
        tVal.tv_usec = (uiMS * 1000);
    }

    m_pExitEvent = event_new(getBase(), 
        -1, EV_PERSIST, exitMonitorCB, this);
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

/************************************************************************
* Function name:Start
* Description  :启动事件循环
* IN           :NONE
* OUT          :NONE
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int CSockPairEvent::Start(void)
{
    int iRtn = Q_RTN_OK;

    if (RunStatus_Error == getRunStatus())
    {
        return Q_RTN_FAILE;
    }

    setRunStatus(RunStatus_Starting);

    //初始化main event    
    iRtn = initMainEvent();
    if (Q_RTN_OK != iRtn)
    {
        setRunStatus(RunStatus_Error);
        return iRtn;
    }

    //初始化assist event  
    iRtn = initAssistEvent();
    if (Q_RTN_OK != iRtn)
    {
        setRunStatus(RunStatus_Error);
        return iRtn;
    }

    //退出监控
    iRtn = initExitMonitor(100);
    if (Q_RTN_OK != iRtn)
    {
        setRunStatus(RunStatus_Error);
        return iRtn;
    }

    if (!onStartUp())
    {
        setRunStatus(RunStatus_Error);
        return iRtn;
    }

    setRunStatus(RunStatus_Runing);
    event_base_dispatch(m_pBase);
    setRunStatus(RunStatus_Stopped);

    m_Mutex.Lock();
    m_Cond.Signal();
    m_Mutex.unLock();

    return Q_RTN_OK;
}

/************************************************************************
* Function name:Stop
* Description  :停止事件循环
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
void CSockPairEvent::Stop(void)
{
    if (getIsRun())
    {
        m_Mutex.Lock();
        setRunStatus(RunStatus_Stopping);
        m_Cond.Wait(&m_Mutex);
        m_Mutex.unLock();
    }
}

void CSockPairEvent::setRunStatus(RunStatus emStatus)
{
    m_emStatus = emStatus;
}

RunStatus CSockPairEvent::getRunStatus(void) const
{
    return (RunStatus)m_emStatus;
}

void CSockPairEvent::setRunOnStop(bool bRun)
{
    m_bRunOnStop = bRun;
}

bool CSockPairEvent::getRunOnStop(void) const
{
    return m_bRunOnStop;
}

bool CSockPairEvent::getError(void) const
{
    return ((RunStatus_Error == getRunStatus()) ? true : false);
}

bool CSockPairEvent::getIsRun(void) const
{
    return ((RunStatus_Runing == getRunStatus()) ? true : false);
}

/************************************************************************
* Function name:waitForStarted
* Description  :等待进入事件循环
* IN           :NONE
* OUT          :NONE
* Return       :bool
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
bool CSockPairEvent::waitForStarted(void) const
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
}

int CSockPairEvent::initAssistEvent(void)
{
    int iRtn = Q_RTN_OK;

    m_pAssistBev = bufferevent_socket_new(getBase(), 
        m_objAssistSockPair.getReadFD(), BEV_OPT_CLOSE_ON_FREE);
    if (NULL == m_pAssistBev)
    {
        Q_Printf("%s", "bufferevent_socket_new error.");

        return Q_RTN_FAILE;
    }  

    iRtn = m_objAssistBuffer.setBuffer(m_pAssistBev);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "set main buffer error.");

        return iRtn;
    }

    bufferevent_setcb(m_pAssistBev, assistReadCB, NULL, eventCB, this);
    iRtn = bufferevent_enable(m_pAssistBev, EV_READ);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "bufferevent_enable error.");

        return iRtn;
    }

    return Q_RTN_OK;
}

int CSockPairEvent::initMainEvent(void)
{
    int iRtn = Q_RTN_OK;

    m_pMainBev = bufferevent_socket_new(getBase(), 
        m_objMainSockPair.getReadFD(), BEV_OPT_CLOSE_ON_FREE);
    if (NULL == m_pMainBev)
    {
        Q_Printf("%s", "bufferevent_socket_new error.");

        return Q_RTN_FAILE;
    }  

    iRtn = m_objMainBuffer.setBuffer(m_pMainBev);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "set main buffer error.");

        return iRtn;
    }

    bufferevent_setcb(m_pMainBev, mainReadCB, NULL, eventCB, this);
    iRtn = bufferevent_enable(m_pMainBev, EV_READ);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "bufferevent_enable error.");

        return iRtn;
    }

    return Q_RTN_OK;
}

int CSockPairEvent::sendMainMsg(const char *pszBuff, const size_t &iSize) const
{
    return m_objMainSockPair.Write(pszBuff, iSize);
}

int CSockPairEvent::sendAssistMsg(const char *pszBuff, const size_t &iSize) const
{
    return m_objAssistSockPair.Write(pszBuff, iSize);
}

void CSockPairEvent::eventCB(struct bufferevent *, short event, void *arg)
{
    CSockPairEvent *pParam = (CSockPairEvent*)arg;
    int iSockError = EVUTIL_SOCKET_ERROR();

    if (event & BEV_EVENT_TIMEOUT)
    {
        return;
    }

    if (event & BEV_EVENT_ERROR)
    {
#ifdef Q_OS_WIN32
        if (WSA_IO_PENDING == iSockError) // WSAEWOULDBLOCK
        {
            return;
        }
#else
        if (EAGAIN == iSockError)
        {
            return;
        }
#endif
    }

    pParam->setRunStatus(RunStatus_Stopping);    
    Q_SYSLOG(LOGLV_ERROR, 
        "an event error happend, event is %d. error code %d, message %s exit loop",
         event, iSockError, evutil_socket_error_to_string(iSockError));
}

void CSockPairEvent::assistReadCB(struct bufferevent *, void *arg)
{
    CSockPairEvent *pParam = (CSockPairEvent*)arg;
    pParam->onAssistRead(pParam->getAssistBuffer());
}

void CSockPairEvent::mainReadCB(struct bufferevent *, void *arg)
{
    CSockPairEvent *pParam = (CSockPairEvent*)arg;
    pParam->onMainRead(pParam->getMainBuffer());
}

void CSockPairEvent::exitMonitorCB(evutil_socket_t, short, void *arg)
{
    CSockPairEvent *pParam = (CSockPairEvent*)arg;

    switch(pParam->getRunStatus())
    {
    case RunStatus_Stopping:
        {
            if (!pParam->getRunOnStop())
            {
                Q_Printf("ready stop thread %u.", Q_ThreadId());
                pParam->onStop();
                pParam->setRunOnStop(true);
            }
        }
        break;

    case RunStatus_Stopped:
        {
            Q_Printf("stop thread %u successfully.", Q_ThreadId());
            event_base_loopbreak(pParam->getBase());
        }
        break;

    default:
        break;
    }
}
