--[[
主lua
--]]

--路径添加
local luaDir = Q_GetModulPath() .. "Lua" .. Q_GetPathSeparator()
package.path = package.path .. ";" .. luaDir .. "Public"..Q_GetPathSeparator().."?.lua"
package.path = package.path .. ";" .. luaDir .. "Game"..Q_GetPathSeparator().."?.lua"

require("InitModule")

local tNowDay = os.date("*t", time)

if not g_objSessionManager then
    g_objSessionManager = nil
end

--[[
描述：服务启动成功后调用
参数：
返回值： 无
--]]
function Lua_OnStartUp(objSessionManager)
    g_objSessionManager = objSessionManager
    OnGameEvent(GameEvent_StartUp)
end

--[[
描述：服务关闭时调用
参数：
返回值：无
--]]
function Lua_OnShutDown()
    OnGameEvent(GameEvent_ShutDown)    
end

--[[
描述：连接可读事件
参数：
返回值：无
--]]
function Lua_OnRead(pszMessage, uiLens)
    Debug(string.sub(pszMessage, 1, uiLens))
    local tbMessage = cjson.decode(string.sub(pszMessage, 1, uiLens))
    local iOpCode = tbMessage[Protocol_Request]
    if not iOpCode then
        Debug("can't find protocol, close this link.")
        g_objSessionManager:closeCurLink()

        return
    end
    
    local objCurSession = g_objSessionManager:getCurSession()
    if not objCurSession then
        Debug("current session is nil.")
        g_objSessionManager:closeCurLink()

        return
    end

    local bServerLinker = objCurSession:isServerLinker()
    if not bServerLinker then
        local iStatus = objCurSession:getStatus()

        --如果为登陆，则除了登陆请求，其他的都不处理
        if (LinkStatus_LogIned ~= iStatus) 
            and (CS_LogIn ~= iOpCode) then
                Debug("not logined.")
                return
        end
        
        --如果已经登陆成功，再发送登陆请求则不处理
        if (LinkStatus_LogIned == iStatus) 
            and (CS_LogIn == iOpCode) then
                Debug("already logined.")
                return
        end
    end
    
    Debug("protocol is " .. iOpCode)    
    OnNetEvent(iOpCode, tbMessage)    
end

--[[
描述：定时器调用
参数：
返回值：无
--]]
function Lua_OnTimer()
    local uiCount = g_objSessionManager:getCount()
    local uiClick = g_objSessionManager:getTimer()
    local uiElapseTime = uiClick * uiCount
    local uiOneSecond = 1000
    
    --每帧处理
    OnGameEvent(GameEvent_FPS, uiClick)
    
    --1秒
    if 0 == (uiElapseTime % uiOneSecond) then
        OnDelayEvent()
        OnGameEvent(GameEvent_1Second)        
        
        --检查变天
        local tDay = os.date("*t", time)
        if (tDay.year ~= tNowDay.year) 
            or (tDay.month ~= tNowDay.month) 
            or (tDay.day ~= tNowDay.day) then
                --变天事件
                tNowDay = tDay
                Debug("day changed.")
                
                OnGameEvent(GameEvent_DayChange)
        end
    end
    
    --5秒
    if 0 == (uiElapseTime % (uiOneSecond * 5)) then
        OnGameEvent(GameEvent_5Second)
    end
    
    --10秒
    if 0 == (uiElapseTime % (uiOneSecond * 10)) then
        OnGameEvent(GameEvent_10Second)
    end
    
    --1分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60)) then
        OnGameEvent(GameEvent_1Minute)
    end
    
    --5分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 5)) then
        OnGameEvent(GameEvent_5Minute)
        collectgarbage("collect")
    end
    
    --10分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 10)) then
        OnGameEvent(GameEvent_10Minute)
    end    
end

--[[
描述：连接断开时调用
参数：
返回值：无
--]]
function Lua_OnClose()
    OnGameEvent(GameEvent_LogOut)
end

--[[
描述：服务器间连接成功时调用
参数：
返回值：无
--]]
function Lua_OnLinkedServer(objSession)
    local tRegSV = {}
    tRegSV[Protocol_Request] = Server_RegServer
    tRegSV[Protocol_ServerID] = getServerID()
    
    local strMsg = cjson.encode(tRegSV)
    g_objSessionManager:sendToByID(objSession:getSessionID(), strMsg, string.len(strMsg))
end
