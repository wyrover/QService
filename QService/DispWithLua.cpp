
#include "DispWithLua.h"

#define LUA_EVENT_ONSTARTUP   "Lua_OnStartUp"
#define LUA_EVENT_ONSHUTDOWN  "Lua_OnShutDown"
#define LUA_EVENT_ONCLOSE     "Lua_OnClose"
#define LUA_EVENT_ONTIMER     "Lua_OnTimer"
#define LUA_EVENT_ONSOCKREAD   "Lua_OnSockRead"
#define LUA_EVENT_ONCONNECTED "Lua_OnConnected"
#define LUA_EVENT_ONHTTPREAD  "Lua_OnHttpRead"
#define LUA_EVENT_ONLINKEDOTHER  "Lua_OnLinkedOther"
#define LUA_EVENT_ONDEBUG  "Lua_OnDebug"

SINGLETON_INIT(CDisposeEvent)
CDisposeEvent objDisposeEvent;

CDisposeEvent::CDisposeEvent(void) : m_pLua(NULL),
    m_objReg2Lua()
{
     
}

CDisposeEvent::~CDisposeEvent(void)
{
    if (NULL != m_pLua)
    {
        Q_Printf("%s", "close lua vm...");
        lua_close(m_pLua);
        m_pLua = NULL;
    }
}

int CDisposeEvent::initLua(const std::string &strLua)
{
    m_pLua = luaL_newstate();
    if (NULL == m_pLua)
    {
        return Q_ERROR_ALLOCMEMORY;
    }

    luaL_openlibs(m_pLua);

    m_objReg2Lua.setLState(m_pLua);
    m_objReg2Lua.Register();

    int iRtn = luaL_dofile(m_pLua, strLua.c_str());
    if (Q_RTN_OK != iRtn)
    {
        const char *pError = lua_tostring(m_pLua, -1);
        std::string strLuaError = ((NULL == pError) ? "" : pError);
        if (NULL != m_pLua)
        {
            lua_close(m_pLua);
            m_pLua = NULL;
        }

        Q_Printf("%s", strLuaError.c_str());
    }

    return iRtn;
}

bool CDisposeEvent::onSerciveStartUp(void)
{
    try
    {
        CSessionManager::getSingletonPtr()->setLua(m_pLua);
        luabridge::getGlobal(m_pLua, LUA_EVENT_ONSTARTUP)(CSessionManager::getSingletonPtr(), 
            CSessionManager::getSingletonPtr()->getNetBinary(), 
            CSessionManager::getSingletonPtr()->getSerializeBinary(), 
            CEncrypt::getSingletonPtr());
    }
    catch(luabridge::LuaException &e)
    {
        Q_Printf("%s", e.what());
        Q_LOG(LOGLV_ERROR, "%s", e.what());

        return false;
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_LOG(LOGLV_ERROR, "exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());

        return false;
    }

    return true;
}

void CDisposeEvent::onSerciveShutDown(void)
{
    try
    {
        luabridge::getGlobal(m_pLua, LUA_EVENT_ONSHUTDOWN)();        
    }
    catch(luabridge::LuaException &e)
    {
        Q_Printf("%s", e.what());
        Q_LOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_LOG(LOGLV_ERROR, "exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
    }
}

void CDisposeEvent::onSockClose(class CSession *pSession)
{
    try
    {
        luabridge::getGlobal(m_pLua, LUA_EVENT_ONCLOSE)(pSession);
    }
    catch(luabridge::LuaException &e)
    {
        Q_Printf("%s", e.what());
        Q_LOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_LOG(LOGLV_ERROR, "exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
    }
}

void CDisposeEvent::onTimerEvent(void)
{
    try
    {
        luabridge::getGlobal(m_pLua, LUA_EVENT_ONTIMER)();
    }
    catch(luabridge::LuaException &e)
    {
        Q_Printf("%s", e.what());
        Q_LOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_LOG(LOGLV_ERROR, "exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
    }
}

void CDisposeEvent::onSockRead(const char *pszMsg, const size_t &iLens)
{
    try
    {
        CSessionManager::getSingletonPtr()->getNetBinary()->setBuffer(pszMsg, iLens);
        luabridge::getGlobal(m_pLua, LUA_EVENT_ONSOCKREAD)();
    }
    catch(luabridge::LuaException &e)
    {
        Q_Printf("%s", e.what());
        Q_LOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_LOG(LOGLV_ERROR, "exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
    }
}

void CDisposeEvent::onDebug(const char *pszMsg, const size_t &iLens)
{
    try
    {
        CSessionManager::getSingletonPtr()->getNetBinary()->setBuffer(pszMsg, iLens);
        luabridge::getGlobal(m_pLua, LUA_EVENT_ONDEBUG)();
    }
    catch(luabridge::LuaException &e)
    {
        Q_Printf("%s", e.what());
        Q_LOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_LOG(LOGLV_ERROR, "exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
    }
}

void CDisposeEvent::onConnected(class CSession *pSession)
{
    try
    {
        luabridge::getGlobal(m_pLua, LUA_EVENT_ONCONNECTED)(pSession);
    }
    catch(luabridge::LuaException &e)
    {
        Q_Printf("%s", e.what());
        Q_LOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_LOG(LOGLV_ERROR, "exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
    }
}

void CDisposeEvent::onHttpRead(class CHttpParser *pBuffer)
{
    try
    {
        luabridge::getGlobal(m_pLua, LUA_EVENT_ONHTTPREAD)(pBuffer);
    }
    catch(luabridge::LuaException &e)
    {
        Q_Printf("%s", e.what());
        Q_LOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_LOG(LOGLV_ERROR, "exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
    }
}

void CDisposeEvent::onLinkedOther(class CSession *pSession)
{
    try
    {
        luabridge::getGlobal(m_pLua, LUA_EVENT_ONLINKEDOTHER)(pSession);
    }
    catch(luabridge::LuaException &e)
    {
        Q_Printf("%s", e.what());
        Q_LOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_LOG(LOGLV_ERROR, "exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
    }
}
