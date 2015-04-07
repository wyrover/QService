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

#include "Log.h"
#include "Exception.h"
#include "SockPairEventParam.h"
#include "QString.h"

struct LogerInfo
{
    CSockPair *pPair;
    CLoger *pLoger;
    struct bufferevent *pBev;
    CEventBuffer *pBuffer;
    void FreeAll()
    {
        Q_SafeDelete(pPair);        
        Q_SafeDelete(pBuffer);
        if (NULL != pBev)
        {
            bufferevent_free(pBev);
            pBev = NULL;
        }
        Q_SafeDelete(pLoger);
    };
    LogerInfo() : pPair(NULL), pLoger(NULL), pBev(NULL), pBuffer(NULL)
    {
    };
};

CLog::CLog(void)
{
    setTcpParam(&m_lstLoger);
    setExitParam(&m_lstLoger);
}

Q_SOCK CLog::addLoger(CLoger *pLoger)
{
    if (NULL == pLoger)
    {
        return Q_INVALID_SOCK;
    }

    if (RunStatus_Runing != getRunStatus())
    {
        Q_Printf("%s", "loger not run.");
        Q_SafeDelete(pLoger);

        return Q_INVALID_SOCK;
    }

    LogerInfo objLogInfo;

    objLogInfo.pLoger = pLoger;
    objLogInfo.pPair = new(std::nothrow) CSockPair();
    if (NULL == objLogInfo.pPair)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);
        objLogInfo.FreeAll();

        return Q_INVALID_SOCK;
    }

    objLogInfo.pBuffer = new(std::nothrow) CEventBuffer();
    if (NULL == objLogInfo.pBuffer)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);
        objLogInfo.FreeAll();

        return Q_INVALID_SOCK;
    }
    
    m_objMutex.Lock();
    int iRtn = sendTcpMsg((const char*)(&objLogInfo), sizeof(objLogInfo));
    m_objMutex.unLock();
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "send message error");
        objLogInfo.FreeAll();

        return Q_INVALID_SOCK;
    }

    return objLogInfo.pPair->getWriteFD();
}

static void LogerReadCB(struct bufferevent *bev, void *arg)
{
    LogerInfo *pLogerInfo = (LogerInfo*)arg;
    size_t iSize = Q_INIT_NUMBER;
    char *pBuf = NULL;

    iSize = pLogerInfo->pBuffer->getTotalLens();
    if (iSize <= 0)
    {
        return;
    }

    pBuf = pLogerInfo->pBuffer->readBuffer(iSize);
    if (NULL == pBuf)
    {
        return;
    }

    pLogerInfo->pLoger->Write(pBuf, iSize);

    pLogerInfo->pBuffer->delBuffer(iSize);
}

void CLog::onTcpRead(SockPairEventParam *pParam)
{
    LogerInfo *pLogerInfo = NULL;

    while(NULL != (pLogerInfo = Q_GetEventValue<LogerInfo>(pParam->pEventBuf)))
    {
        //Ìí¼Ó½øÑ­»·
        pLogerInfo->pBev = bufferevent_socket_new(pParam->pMainBase, 
            pLogerInfo->pPair->getReadFD(), BEV_OPT_CLOSE_ON_FREE);
        if (NULL == pLogerInfo->pBev)
        {
            Q_Printf("%s", "bufferevent_socket_new error.");
            pLogerInfo->FreeAll();
            Q_SafeDelete(pLogerInfo);

            continue;
        }

        if (Q_RTN_OK != pLogerInfo->pBuffer->setBuffer(pLogerInfo->pBev))
        {
            Q_Printf("%s", "set buffer error.");
            pLogerInfo->FreeAll();
            Q_SafeDelete(pLogerInfo);

            continue;
        }

        bufferevent_setcb(pLogerInfo->pBev, LogerReadCB, NULL, NULL, pLogerInfo);
        if (Q_RTN_OK != bufferevent_enable(pLogerInfo->pBev, EV_READ))
        {
            Q_Printf("%s", "bufferevent_enable error.");
            pLogerInfo->FreeAll();
            Q_SafeDelete(pLogerInfo);

            continue;
        }

        ((std::list<LogerInfo *>*)(pParam->pUserDate))->push_back(pLogerInfo);
    }
}

void CLog::onStop(SockPairEventParam *pParam)
{
    std::list<LogerInfo *>::iterator itTask;
    std::list<LogerInfo *> *plstUserData = (std::list<LogerInfo *> *)(pParam->pUserDate);

    for (itTask = plstUserData->begin(); plstUserData->end() != itTask; itTask++)
    {
        (*itTask)->FreeAll();
        Q_SafeDelete((*itTask));
    }

    plstUserData->clear();

    setRunStatus(RunStatus_Stopped);
}
