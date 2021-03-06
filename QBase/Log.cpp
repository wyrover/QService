
#include "Log.h"
#include "Exception.h"
#include "QString.h"

SINGLETON_INIT(CLog)
CLog objLog;

#define LOG_EVTIME (5 * 60 * 1000)

struct LogerInfo
{
    CSockPair *pPair;
    CLoger *pLoger;
    struct bufferevent *pBev;
    CEventBuffer *pBuffer;
    CTcpParser *pParser;
    void FreeAll()
    {
        Q_SafeDelete(pPair);
        Q_SafeDelete(pBuffer);
        if (NULL != pBev)
        {
            bufferevent_free(pBev);
            pBev = NULL;
        }
    };
    LogerInfo() : pPair(NULL), pLoger(NULL), pBev(NULL), pBuffer(NULL), pParser(NULL)
    {
    };
};

CLog::CLog(void) : m_pTimeEvent(NULL)
{
    (void)setTimer(LOG_EVTIME);
}

CLog::~CLog(void)
{
    if (NULL != m_pTimeEvent)
    {
        event_free(m_pTimeEvent);
        m_pTimeEvent = NULL;
    }
}

int CLog::setTimer(unsigned int uiMS)
{
    if (Q_INIT_NUMBER == uiMS)
    {
        return Q_RTN_OK;
    }

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

    m_pTimeEvent = event_new(getBase(), 
        -1, EV_PERSIST, timerCB, &m_lstLoger);
    if (NULL == m_pTimeEvent)
    {
        Q_Printf("%s", "event_new error");

        return Q_RTN_FAILE;
    }

    if (Q_RTN_OK != event_add(m_pTimeEvent, &tVal))
    {
        Q_Printf("%s", "event_add error");

        event_free(m_pTimeEvent);
        m_pTimeEvent = NULL;

        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

void CLog::addLoger(CLoger *pLoger)
{
    if (NULL == pLoger)
    {
        return;
    }

    if (RUNSTATUS_RUNING != getRunStatus())
    {
        Q_Printf("%s", "loger not run.");

        return;
    }

    LogerInfo objLogInfo;

    objLogInfo.pLoger = pLoger;
    objLogInfo.pPair = new(std::nothrow) CSockPair();
    if (NULL == objLogInfo.pPair)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);
        objLogInfo.FreeAll();

        return;
    }

    pLoger->setSock(objLogInfo.pPair->getWriteFD());
    objLogInfo.pBuffer = new(std::nothrow) CEventBuffer();
    if (NULL == objLogInfo.pBuffer)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);
        objLogInfo.FreeAll();

        return;
    }
    
    m_objMutex.Lock();
    int iRtn = sendMainMsg((const char*)(&objLogInfo), sizeof(objLogInfo));
    m_objMutex.unLock();
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "send message error");
        objLogInfo.FreeAll();

        return;
    }

    return;
}

void CLog::timerCB(evutil_socket_t, short, void *arg)
{
    std::list<LogerInfo *>::iterator itLoger;
    std::list<LogerInfo *> *pLoger = (std::list<LogerInfo *> *)arg;
    for (itLoger = pLoger->begin(); pLoger->end() != itLoger; itLoger++)
    {
        (void)(*itLoger)->pLoger->Check();
    }
}

void CLog::LogerReadCB(struct bufferevent *, void *arg)
{
    LogerInfo *pLogerInfo = (LogerInfo*)arg;
    CTcpParser *pParser = pLogerInfo->pParser;

    while(true)
    {
        const char *pszBuf = pParser->parsePack(pLogerInfo->pBuffer);
        if (NULL == pszBuf)
        {
            break;
        }

        pLogerInfo->pLoger->Write(pszBuf, pParser->getBufLens());
        (void)pLogerInfo->pBuffer->delBuffer(pParser->getParsedLens());
    }
}

void CLog::onMainRead(CEventBuffer *pBuffer)
{
    LogerInfo *pLogerInfo = NULL;

    while(NULL != (pLogerInfo = Q_GetEventValue<LogerInfo>(pBuffer)))
    {
        //���ӽ�ѭ��
        pLogerInfo->pBev = bufferevent_socket_new(getBase(), 
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

        pLogerInfo->pParser = &m_objTcpParser;
        bufferevent_setcb(pLogerInfo->pBev, LogerReadCB, NULL, NULL, pLogerInfo);
        if (Q_RTN_OK != bufferevent_enable(pLogerInfo->pBev, EV_READ))
        {
            Q_Printf("%s", "bufferevent_enable error.");
            pLogerInfo->FreeAll();
            Q_SafeDelete(pLogerInfo);

            continue;
        }

        m_lstLoger.push_back(pLogerInfo);
    }
}

void CLog::onStop(void)
{
    std::list<LogerInfo *>::iterator itTask;
    for (itTask = m_lstLoger.begin(); m_lstLoger.end() != itTask; itTask++)
    {
        (*itTask)->FreeAll();
        Q_SafeDelete((*itTask));
    }

    m_lstLoger.clear();

    setRunStatus(RUNSTATUS_STOPPED);
    Q_Printf("%s", "log system stopped.");
}
