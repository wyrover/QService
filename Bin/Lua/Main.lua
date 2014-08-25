--[[
主lua
--]]

--路径添加
local luaDir = Q_GetModulPath() .. "Lua" .. Q_GetPathSeparator()
package.path = package.path .. ";" .. luaDir .. "?.lua"

require("Game/InitModule")

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
    local iProtocol = tbMessage[ProtocolStr_Request]
    if not iProtocol then
        Debug("can't find protocol, close this link.")
        g_objSessionManager:closeCurLink()

        return
    end
    
    Debug("protocol is " .. iProtocol)    
    OnNetEvent(iProtocol, tbMessage)    
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
    
    --1小时
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 60)) then
        OnGameEvent(GameEvent_1Hour)
    end
end

--[[
描述：连接断开时调用
参数：
返回值：无
--]]
function Lua_OnClose()
    OnGameEvent(GameEvent_Close)
end

--[[
描述：服务器间连接成功时调用
参数：
返回值：无
--]]
function Lua_OnLinkedServer(objSession)
    
end
