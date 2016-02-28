#include "Worker.h"

int g_iChecked = 0;
CQMutex g_objWorkerMutex;
SINGLETON_INIT(CWorker)
CWorker m_objWorker;

CWorker::CWorker(void) : m_pEvent(NULL), m_pLua(NULL)
{

}

CWorker::~CWorker(void)
{
    if (NULL != m_pEvent)
    {
        event_free(m_pEvent);
        m_pEvent = NULL;
    }

    if (NULL != m_pLua)
    {
        lua_close(m_pLua);
        m_pLua = NULL;
    }
}

bool getChecked(void)
{
    return 1 == g_iChecked ? true : false;
}

void CWorker::regMFCFunc(void)
{
    luabridge::getGlobalNamespace(m_pLua)
        .addFunction("showMfcMsg", showMfcMsg)
        .addFunction("showLuaMemory", showLuaMemory)
        .addFunction("getChecked", getChecked);
}

bool CWorker::initLua(void)
{
    m_pLua = luaL_newstate();
    if (NULL == m_pLua)
    {
        setRunStatus(RUNSTATUS_ERROR);
        return false;
    }

    luaL_openlibs(m_pLua);
    m_objRegLua.setLState(m_pLua);
    m_objRegLua.Register();
    regMFCFunc();

    std::string strLuaPath = Q_FormatStr("%sToolLua%sTool.lua", 
        g_acModulPath, Q_PATH_SEPARATOR);
    int iRtn = luaL_dofile(m_pLua, strLuaPath.c_str());
    if (Q_RTN_OK != iRtn)
    {
        const char *pError = lua_tostring(m_pLua, -1);
        std::string strLuaError = ((NULL == pError) ? "" : pError);
        if (NULL != m_pLua)
        {
            lua_close(m_pLua);
            m_pLua = NULL;
        }

        showMfcMsg(strLuaError.c_str(), strLuaError.size());
        setRunStatus(RUNSTATUS_ERROR);

        return false;
    }

    CSessionManager::getSingletonPtr()->setLua(m_pLua);

    return true;
}

void CWorker::workThreadTimerCB(evutil_socket_t, short event, void *arg)
{
    CWorker *pWorker = (CWorker*)arg;

    try
    {
        CSessionManager::getSingletonPtr()->addCount();
        CLockThis objLock(&g_objWorkerMutex);
        luabridge::getGlobal(pWorker->getLua(), "Lua_onTime")();
    }
    catch(luabridge::LuaException &e)
    {
        showMfcMsg(e.what(), strlen(e.what()));
    }
}

void CWorker::setTimer(unsigned int uiMS)
{
    timeval tVal;

    CSessionManager::getSingletonPtr()->setTimer(uiMS);

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

    m_pEvent = event_new(getBase(), 
        -1, EV_PERSIST, workThreadTimerCB, this);
    if (NULL == m_pEvent)
    {
        Q_Printf("%s", "event_new error");
        setRunStatus(RUNSTATUS_ERROR);

        return;
    }

    if (Q_RTN_OK != event_add(m_pEvent, &tVal))
    {
        Q_Printf("%s", "event_add error");
        setRunStatus(RUNSTATUS_ERROR);
        event_free(m_pEvent);
        m_pEvent = NULL;

        return;
    }
}

void CWorker::mainReadCB(struct bufferevent *bev, void *arg)
{
    CWorker *pWorker = (CWorker*)arg;
    CSessionManager *pSessionManager = CSessionManager::getSingletonPtr();
    CSession *pSession = pSessionManager->getSession(bev);
    CTcpParser *pParser = pWorker->getTcpParser();
    if (NULL == pSession)
    {
        Q_Printf("%s", "get session error.");
        bufferevent_free(bev);

        return;
    }

    size_t iTmpLens = Q_INIT_NUMBER;
    pSessionManager->setCurSession(pSession);
    while(true)
    {
        const char *pszBuf = pParser->parsePack(pSession->getBuffer());
        if (NULL == pszBuf)
        {
            break;
        }

        const char *pszTmp = CClientEncrypt::getSingletonPtr()->Decode(pszBuf, pParser->getBufLens(), iTmpLens);
        pSessionManager->getNetBinary()->setBuffer(pszTmp, iTmpLens);

        try
        {
            CLockThis objLock(&g_objWorkerMutex);
            luabridge::getGlobal(pWorker->getLua(), "Lua_onMainRead")();
        }
        catch(luabridge::LuaException &e)
        {
            showMfcMsg(e.what(), strlen(e.what()));
        }

        if (SESSSTATUS_CLOSED != pSession->getStatus())
        {
            (void)pSession->getBuffer()->delBuffer(pParser->getParsedLens());
        }
        else
        {
            break;
        }
    }
    pSessionManager->setCurSession(NULL);
}

void CWorker::mainEventCB(struct bufferevent *bev, short event, void *arg)
{
    CWorker *pWorker = (CWorker*)arg;
    CSessionManager *pSessionManager = CSessionManager::getSingletonPtr();
    CSession *pSession = pSessionManager->getSession(bev);
    if (NULL == pSession)
    {
        bufferevent_free(bev);

        return;
    }

    pSessionManager->dellSession(bev);
    bufferevent_free(bev);

    enableLinkButt();
}

void CWorker::onMainRead(CEventBuffer *pBuffer)
{
    struct bufferevent *pBev = NULL;
    CSession *pSession = NULL;

    while(true)
    {
        const char *pszBuf = m_objTcpParser.parsePack(pBuffer);
        if (NULL == pszBuf)
        {
            break;
        }

        evutil_socket_t sock = *((evutil_socket_t*)pszBuf);
        pBuffer->delBuffer(m_objTcpParser.getParsedLens());
        if (Q_INVALID_SOCK == sock)
        {
            continue;
        }

        (void)evutil_make_socket_nonblocking(sock);
        pBev = bufferevent_socket_new(getBase(), sock, BEV_OPT_CLOSE_ON_FREE);
        if (NULL == pBev)
        {
            enableLinkButt();
            evutil_closesocket(sock);
            continue;
        }
        if (Q_RTN_OK != CSessionManager::getSingletonPtr()->addSession(pBev))
        {
            enableLinkButt();
            evutil_closesocket(sock);
            bufferevent_free(pBev);

            continue;
        }

        bufferevent_setcb(pBev, mainReadCB, NULL, mainEventCB, 
            this);
        if (Q_RTN_OK != bufferevent_enable(pBev, EV_READ | EV_WRITE))
        {
            enableLinkButt();
            CSessionManager::getSingletonPtr()->dellSession(pBev);
            evutil_closesocket(sock);
            bufferevent_free(pBev);

            continue;
        }

        pSession = CSessionManager::getSingletonPtr()->getSession(pBev);
        pSession->setType(STYPE_TCPCLIENT);
        pSession->setStatus(SESSSTATUS_LINKED);
    }
}

void CWorker::onAssistRead(CEventBuffer *pEventBuffer)
{
    WorkerCommand stCommand;
    while(Q_GetEventValue<WorkerCommand>(pEventBuffer, stCommand))
    {
        switch(stCommand.emType)
        {
        case WCOMM_CLOSEMAIN:
            {
                CSessionManager::getSingletonPtr()->closeLinkByID(stCommand.sock);
                enableLinkButt();
            }
            break;

        default:
            break;
        }
    }
}

bool CWorker::onStartUp(void)
{
    try
    {
        CLockThis objLock(&g_objWorkerMutex);
        luabridge::getGlobal(getLua(), "Lua_onStartUp")(CSessionManager::getSingletonPtr(), 
            CSessionManager::getSingletonPtr()->getNetBinary(), CEncrypt::getSingletonPtr());
    }
    catch(luabridge::LuaException &e)
    {
        showMfcMsg(e.what(), strlen(e.what()));

        return false;
    }

    return true;
}

void CWorker::onStop(void)
{
    setRunStatus(RUNSTATUS_STOPPED);
}
