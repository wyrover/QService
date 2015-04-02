--[[
ping
--]]

local PingTimeOut = 5 * 60 * 1000

--[[
描述：ping
参数：
返回值：无
--]]
local function CSPing(tbMessage)
    local objCurSession = g_objSessionMgr:getCurSession()
    local iStatus = objCurSession:getStatus()
    if (SessionStatus.Playing ~= iStatus) 
        and (SessionStatus.GM ~= iStatus) then
        return
    end
    
    objCurSession:setPing(g_objSessionMgr:getCount())
    g_objSessionMgr:sendToCur(Protocol.SC_Ping, "", 0)
end
regNetEvent(Protocol.CS_Ping, CSPing)

--[[
描述：ping检查
参数：
返回值：无
--]]
local function CheckPing()
    g_objSessionMgr:checkPing(PingTimeOut)
end
regGameEvent(GameEvent.FiveMinute, CheckPing)
