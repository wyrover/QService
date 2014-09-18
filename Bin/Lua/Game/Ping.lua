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
    local objCurSession = g_objSessionManager:getCurSession()
    if 0 == string.len(objCurSession:getID()) then
        return
    end
    
    objCurSession:setPing(g_objSessionManager:getCount())
end
RegNetEvent(Protocol.CS_Ping, CSPing)

--[[
描述：ping检查
参数：
返回值：无
--]]
local function CheckPing()
    g_objSessionManager:checkPing(PingTimeOut)
end
RegGameEvent(GameEvent.FiveMinute, "CheckPing", CheckPing)
