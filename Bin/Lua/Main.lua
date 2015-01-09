--[[
主lua
--]]

--路径添加
local luaDir = Q_GetModulPath() .. "Lua" .. Q_GetPathSeparator()
package.path = package.path .. ";" .. luaDir .. "?.lua"

cjson = require("cjson")
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
    math.randomseed(tonumber(tostring(os.time()):reverse():sub(1,6)))
    
    onGameEvent(GameEvent.Start)
    onGameEvent(GameEvent.Started)
end

--[[
描述：服务关闭时调用
参数：
返回值：无
--]]
function Lua_OnShutDown()
    onGameEvent(GameEvent.ShutDown)    
end

--[[
描述：连接可读事件
参数：
返回值：无
--]]
function Lua_OnRead(objMessageTrans)
    Debug(string.format("recv message lens %d", objMessageTrans:getLens()))
    --[[
    Debug(string.sub(pszMessage, 1, uiLens))
    local tbMessage = cjson.decode(string.sub(pszMessage, 1, uiLens))
    local iProtocol = tbMessage[ProtocolStr_Request]
    if not iProtocol then
        Debug("can't find protocol, close this link.")
        g_objSessionManager:closeCurLink()

        return
    end
    
    local objCurSession = g_objSessionManager:getCurSession()   
    if not objCurSession:isServerLinker() then
        if g_ProtocolFilterFun then
            local iStatus = objCurSession:getStatus()
            if not g_ProtocolFilterFun(iProtocol, iStatus) then
                Q_LOG(LOGLV_WARN, string.format("session status %d, protocol %d was ignored.", 
                    iStatus, iProtocol))
                return
            end
        end
    end
    
    if g_CheckSVRegFun and (Protocol.System_RegSV ~= iProtocol) then
        if not g_CheckSVRegFun(objCurSession:getSessionID()) then
            Debug(string.format("client id %d not register.", objCurSession:getSessionID()))
            g_objSessionManager:closeCurLink()
            return
        end
    end
    
    Debug("protocol is " .. iProtocol)  
--]]    
    --onNetEvent(iProtocol, tbMessage)
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
    onGameEvent(GameEvent.FPS, uiClick)
    
    --1秒
    if 0 == (uiElapseTime % uiOneSecond) then
        onDelayEvent()
        onGameEvent(GameEvent.OneSecond)        
        
        --检查变天
        local tDay = os.date("*t", time)
        if (tDay.year ~= tNowDay.year) 
            or (tDay.month ~= tNowDay.month) 
            or (tDay.day ~= tNowDay.day) then
                --变天事件
                tNowDay = tDay
                Debug("day changed.")                
                onGameEvent(GameEvent.DayChange)
                
                local iNewWeek = tonumber(os.date("%U", os.time(tDay)))
                local iOldWeek = tonumber(os.date("%U", os.time(tNowDay)))
                if iNewWeek ~= iOldWeek then
                    onGameEvent(GameEvent.WeekChange)
                end            
                if tDay.month ~= tNowDay.month then
                    onGameEvent(GameEvent.MonthChange)
                end
                
                tNowDay = tDay
        end
    end
    
    --5秒
    if 0 == (uiElapseTime % (uiOneSecond * 5)) then
        onGameEvent(GameEvent.FiveSecond)
    end
    
    --10秒
    if 0 == (uiElapseTime % (uiOneSecond * 10)) then
        onGameEvent(GameEvent.TenSecond)
    end
    
    --1分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60)) then
        onGameEvent(GameEvent.OneMinute)
    end
    
    --5分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 5)) then
        onGameEvent(GameEvent.FiveMinute)
        collectgarbage("collect")
    end
    
    --10分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 10)) then
        onGameEvent(GameEvent.TenMinute)
    end
	
    --1小时
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 60)) then
        onGameEvent(GameEvent.OneHour)
    end
end

--[[
描述：连接断开时调用
参数：
返回值：无
--]]
function Lua_OnClose()
    onGameEvent(GameEvent.Close)
end

--[[
描述：服务器间连接成功时调用
参数：
返回值：无
--]]
function Lua_OnLinkedServer(objSession)
    --RequireRegSV(objSession)
end
