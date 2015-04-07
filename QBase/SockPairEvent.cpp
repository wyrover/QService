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

CSockPairEvent::CSockPairEvent(void) : m_bRunOnStop(false),
    m_cStatus(RunStatus_Unknown), m_pEventBase(NULL), m_pBevs(NULL), m_pSockPairs(NULL), 
    m_pBuffers(NULL), m_pParam(NULL), m_pExitEvent(NULL)
{
    int iRtn = Init();
    if (Q_RTN_OK != iRtn)
    {
        setRunStatus(RunStatus_Error);
        freeAll();

        Q_EXCEPTION(iRtn, "%s", "init socket pair event error.");
    }
}

CSockPairEvent::~CSockPairEvent(void)
{
    freeAll();
}

int CSockPairEvent::Init(void)
{
    try
    {
        m_pSockPairs = new(std::nothrow) CSockPair[TYPE_COUNT];
        if (NULL == m_pSockPairs)
        {
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

            return Q_ERROR_ALLOCMEMORY;
        }
    }
    catch(CException &e)
    {
        Q_Printf("get an exception. code %d, message %s", e.getErrorCode(), e.getErrorMsg());

        return e.getErrorCode();
    }

    m_pEventBase = event_base_new();
    if (NULL == m_pEventBase)
    {
        Q_Printf("%s", "event_base_new error.");

        return Q_RTN_FAILE;
    }

    (void)event_base_priority_init(m_pEventBase, Priority_Count);

    m_pBevs = new(std::nothrow) struct bufferevent *[TYPE_COUNT];
    if (NULL == m_pBevs)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

        return Q_ERROR_ALLOCMEMORY;
    }

    for (int i = 0; i < TYPE_COUNT; i++)
    {
        m_pBevs[i] = NULL;
    }    

    m_pBuffers = new(std::nothrow) CEventBuffer[TYPE_COUNT];
    if (NULL == m_pBuffers)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

        return Q_ERROR_ALLOCMEMORY;
    }

    m_pParam = new(std::nothrow) SockPairEventParam[TYPE_COUNT];
    if (NULL == m_pParam)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

        return Q_ERROR_ALLOCMEMORY;
    }    

    for (int i = 0; i < TYPE_COUNT; i++)
    {
        m_pParam[i].emType = (SockPairEventType)i;
        m_pParam[i].pEventBuf = &m_pBuffers[i]; 
        m_pParam[i].pMainBase = m_pEventBase;
        m_pParam[i].pFun = this;
    }

    m_stExitParam.pMainBase = m_pEventBase;
    m_stExitParam.pFun = this;

    return Q_RTN_OK;
}

void CSockPairEvent::freeAll(void)
{
    Q_SafeDelete_Array(m_pSockPairs);

    if (NULL != m_pBevs)
    {
        for (int i = 0; i < TYPE_COUNT; i++)
        {
            if (NULL != m_pBevs[i])
            {
                bufferevent_free(m_pBevs[i]);
                m_pBevs[i] = NULL;
            }
        }
    }
    
    Q_SafeDelete_Array(m_pBevs);
    Q_SafeDelete_Array(m_pBuffers);    
    Q_SafeDelete_Array(m_pParam);

    if (NULL != m_pExitEvent)
    {
        event_free(m_pExitEvent);
        m_pExitEvent = NULL;
    }

    if (NULL != m_pEventBase)
    {
        event_base_free(m_pEventBase);
        m_pEventBase = NULL;
    }
}

int CSockPairEvent::initExitMonitor(unsigned int uiMS)
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

    m_pExitEvent = event_new(getBase(), 
        -1, EV_PERSIST, exitMonitorCB, &m_stExitParam);
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
    //初始化时是否出错
    if (getError())
    {
        return Q_RTN_FAILE;
    }

    int iRtn = Q_RTN_OK;

    //初始化event
    setRunStatus(RunStatus_Starting);
    iRtn = initEvent();
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
    event_base_dispatch(m_pEventBase);
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
    if (getIsRun()
        && NULL != m_pSockPairs)
    {
        m_Mutex.Lock();
        setRunStatus(RunStatus_Stopping);
        m_Cond.Wait(&m_Mutex);
        m_Mutex.unLock();
    }
}

void CSockPairEvent::setRunStatus(RunStatus emStatus)
{
    m_cStatus = emStatus;
}

RunStatus CSockPairEvent::getRunStatus(void)
{
    return (RunStatus)m_cStatus;
}

void CSockPairEvent::setRunOnStop(bool bRun)
{
    m_bRunOnStop = bRun;
}

bool CSockPairEvent::getRunOnStop(void)
{
    return m_bRunOnStop;
}

bool CSockPairEvent::getError(void)
{
    return ((RunStatus_Error == getRunStatus()) ? true : false);
}

bool CSockPairEvent::getIsRun(void)
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
bool CSockPairEvent::waitForStarted(void)
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

void CSockPairEvent::setTcpParam(void *pArg)
{
    m_pParam[TYPE_TCP].pUserDate = pArg;
}

void CSockPairEvent::setOrderParam(void *pArg)
{
    m_pParam[TYPE_ORDER].pUserDate = pArg;
}

void CSockPairEvent::setWebSockParam(void *pArg)
{
    m_pParam[TYPE_WEBSOCK].pUserDate = pArg;
}

void CSockPairEvent::setExitParam(void *pArg)
{
    m_stExitParam.pUserDate = pArg;
}

int CSockPairEvent::initEvent(struct bufferevent **pBev, struct SockPairEventParam *pParam, CSockPair &objPair)
{
    int iRtn = Q_RTN_OK;

    *pBev = bufferevent_socket_new(pParam->pMainBase, 
        objPair.getReadFD(), BEV_OPT_CLOSE_ON_FREE);
    if (NULL == *pBev)
    {
        Q_Printf("%s", "bufferevent_socket_new error.");

        return Q_RTN_FAILE;
    }  

    iRtn = pParam->pEventBuf->setBuffer(*pBev);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "set main buffer error.");

        return iRtn;
    }

    bufferevent_setcb(*pBev, sockPairEventReadCB, NULL, sockPairEventCB, pParam);
    iRtn = bufferevent_enable(*pBev, EV_READ);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "bufferevent_enable error.");

        return iRtn;
    }

    return Q_RTN_OK;
}

int CSockPairEvent::initEvent(void)
{
    for (int i = 0; i < TYPE_COUNT; i++)
    {
        if (Q_RTN_OK != initEvent(&m_pBevs[i], &m_pParam[i], m_pSockPairs[i]))
        {
            return Q_RTN_FAILE;
        }
    }

    return Q_RTN_OK;
}

int CSockPairEvent::sendOrderMsg(const char *pszBuff, const size_t &iSize)
{
    return m_pSockPairs[TYPE_ORDER].Write(pszBuff, iSize);
}

int CSockPairEvent::sendTcpMsg(const char *pszBuff, const size_t &iSize)
{
    return m_pSockPairs[TYPE_TCP].Write(pszBuff, iSize);
}

int CSockPairEvent::sendWebSockMsg(const char *pszBuff, const size_t &iSize)
{
    return m_pSockPairs[TYPE_WEBSOCK].Write(pszBuff, iSize);
}

void CSockPairEvent::sockPairEventCB(struct bufferevent *bev, short event, void *arg)
{
    SockPairEventParam *pParam = (SockPairEventParam*)arg;
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

    event_base_loopbreak(pParam->pMainBase);
    Q_SYSLOG(LOGLV_ERROR, 
        "an event error for bufferevent %d, event is %d. error code %d, message %s exit loop",
        pParam->emType, event, iSockError, evutil_socket_error_to_string(iSockError));
}

void CSockPairEvent::sockPairEventReadCB(struct bufferevent *bev, void *arg)
{
    SockPairEventParam *pParam = (SockPairEventParam*)arg;    

    switch(pParam->emType)
    {
    case TYPE_TCP:
        pParam->pFun->onTcpRead(pParam);
        break;

    case TYPE_ORDER:
        pParam->pFun->onOrderRead(pParam);
        break;

    case TYPE_WEBSOCK:
        pParam->pFun->onWebSockRead(pParam);
        break;

    default:
        Q_Printf("%s", "unkown event type");
        break;
    }
}

void CSockPairEvent::exitMonitorCB(evutil_socket_t, short event, void *arg)
{
    SockPairEventParam *pParam = (SockPairEventParam*)arg;

    switch(pParam->pFun->getRunStatus())
    {
    case RunStatus_Stopping:
        {
            if (!pParam->pFun->getRunOnStop())
            {
                Q_Printf("ready stop thread %u.", Q_ThreadId());
                pParam->pFun->onStop(pParam);
                pParam->pFun->setRunOnStop(true);
            }
        }
        break;

    case RunStatus_Stopped:
        {
            Q_Printf("stop thread %u successfully.", Q_ThreadId());
            event_base_loopbreak(pParam->pMainBase);
        }
        break;

    default:
        break;
    }
}
