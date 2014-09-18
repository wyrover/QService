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

--操作码过滤
if not g_ProtocolFilterFun then
    g_ProtocolFilterFun = nil
end

--检查服务器注册
if not g_CheckSVRegFun then
    g_CheckSVRegFun = nil
end

--[[
描述：服务启动成功后调用
参数：
返回值： 无
--]]
function Lua_OnStartUp(objSessionManager)
    g_objSessionManager = objSessionManager
    OnGameEvent(GameEvent.StartUp)
end

--[[
描述：服务关闭时调用
参数：
返回值：无
--]]
function Lua_OnShutDown()
    OnGameEvent(GameEvent.ShutDown)    
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
    
    local objCurSession = g_objSessionManager:getCurSession()   
    if not objCurSession:isServerLinker() then
        if g_ProtocolFilterFun then
            local iStatus = objCurSession:getStatus()
            if not g_ProtocolFilterFun(iProtocol, iStatus) then
                Q_LOG(LOGLV_WARN, string.format("session status %d, protocol %d was ignored.", iStatus, iProtocol))
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
    OnGameEvent(GameEvent.FPS, uiClick)
    
    --1秒
    if 0 == (uiElapseTime % uiOneSecond) then
        OnDelayEvent()
        OnGameEvent(GameEvent.OneSecond)        
        
        --检查变天
        local tDay = os.date("*t", time)
        if (tDay.year ~= tNowDay.year) 
            or (tDay.month ~= tNowDay.month) 
            or (tDay.day ~= tNowDay.day) then
                --变天事件
                tNowDay = tDay
                Debug("day changed.")
                
                OnGameEvent(GameEvent.DayChange)
        end
    end
    
    --5秒
    if 0 == (uiElapseTime % (uiOneSecond * 5)) then
        OnGameEvent(GameEvent.FiveSecond)
    end
    
    --10秒
    if 0 == (uiElapseTime % (uiOneSecond * 10)) then
        OnGameEvent(GameEvent.TenSecond)
    end
    
    --1分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60)) then
        OnGameEvent(GameEvent.OneMinute)
    end
    
    --5分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 5)) then
        OnGameEvent(GameEvent.FiveMinute)
        collectgarbage("collect")
    end
    
    --10分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 10)) then
        OnGameEvent(GameEvent.TenMinute)
    end
	
    --1小时
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 60)) then
        OnGameEvent(GameEvent.OneHour)
    end
end

--[[
描述：连接断开时调用
参数：
返回值：无
--]]
function Lua_OnClose()
    OnGameEvent(GameEvent.Close)
end

--[[
描述：服务器注册
参数：
返回值：无
--]]
local function RequireRegSV(objSession)
    local iClientID = objSession:getSessionID()
    local tRegSV = {}
    local strCheckID = GetID()
    
    objSession:setCheckID(strCheckID)
    
    tRegSV[ProtocolStr_Request] = Protocol.System_RegSV
    tRegSV[ProtocolStr_ServerID] = getServerID()
    tRegSV[ProtocolStr_CheckID] = strCheckID
    tRegSV[ProtocolStr_ClientID] = iClientID
    
    
    local strMsg = cjson.encode(tRegSV)
    g_objSessionManager:sendToByID(iClientID, strMsg, string.len(strMsg))
end

--[[
描述：服务器间连接成功时调用
参数：
返回值：无
--]]
function Lua_OnLinkedServer(objSession)
    RequireRegSV(objSession)
end
