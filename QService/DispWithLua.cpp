
#include "DispWithLua.h"

#define LUA_EVENT_ONSTARTUP   "Lua_OnStartUp"
#define LUA_EVENT_ONSHUTDOWN  "Lua_OnShutDown"
#define LUA_EVENT_ONCLOSE     "Lua_OnClose"
#define LUA_EVENT_ONTIMER     "Lua_OnTimer"
#define LUA_EVENT_ONSOCKREAD   "Lua_OnSockRead"
#define LUA_EVENT_ONCONNECTED "Lua_OnConnected"
#define LUA_EVENT_ONHTTPREAD  "Lua_OnHttpRead"
#define LUA_EVENT_ONLINKEDOTHER  "Lua_OnLinkedOther"

CDisposeEvent::CDisposeEvent(const char *pszLuaFile) : m_pLua(NULL),
    m_objReg2Lua()
{
    m_pLua = luaL_newstate();
    if (NULL == m_pLua)
    {
        Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s", "luaL_newstate error.");
    }

    luaL_openlibs(m_pLua);

    m_objReg2Lua.setLState(m_pLua);
    m_objReg2Lua.Register();

    int iRtn = luaL_dofile(m_pLua, pszLuaFile);
	if (Q_RTN_OK != iRtn)
	{
		const char *pError = lua_tostring(m_pLua, -1);
		std::string strLuaError = ((NULL == pError) ? "" : pError);
		if (NULL != m_pLua)
		{
			lua_close(m_pLua);
			m_pLua = NULL;
		}

		Q_EXCEPTION(Q_RTN_FAILE, strLuaError.c_str());      
	}    
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

bool CDisposeEvent::onSerciveStartUp(void)
{
    try
    {
        getSessionManager()->setLua(m_pLua);
        luabridge::getGlobal(m_pLua, LUA_EVENT_ONSTARTUP)(getSessionManager(), 
            getSessionManager()->getBinary());
    }
    catch(luabridge::LuaException &e)
    {
        Q_Printf("%s", e.what());
        Q_SYSLOG(LOGLV_ERROR, "%s", e.what());

        return false;
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "exception. code %d, message %s", 
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
        Q_SYSLOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "exception. code %d, message %s", 
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
        Q_SYSLOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "exception. code %d, message %s", 
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
        Q_SYSLOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
    }
}

void CDisposeEvent::onSockRead(const char *pszMsg, const size_t &iLens)
{
    try
    {
        getSessionManager()->getBinary()->setBuffer(pszMsg, iLens);
        luabridge::getGlobal(m_pLua, LUA_EVENT_ONSOCKREAD)();
    }
    catch(luabridge::LuaException &e)
    {
        Q_Printf("%s", e.what());
        Q_SYSLOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "exception. code %d, message %s", 
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
        Q_SYSLOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "exception. code %d, message %s", 
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
        Q_SYSLOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "exception. code %d, message %s", 
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
        Q_SYSLOG(LOGLV_ERROR, "%s", e.what());
    }
    catch(CQException &e)
    {
        Q_Printf("exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
        Q_SYSLOG(LOGLV_ERROR, "exception. code %d, message %s", 
            e.getErrorCode(), e.getErrorMsg());
    }
}
