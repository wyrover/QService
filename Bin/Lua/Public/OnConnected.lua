--[[
链接成功执行
--]]

--[[
描述：链接成功
参数：
返回值：无
--]]
local function clientOnConnected(objSession)
    local strIDCard = getID()
    objSession:setIDCard(strIDCard)
end
regGameEvent(GameEvent.OnConnected, clientOnConnected)

--[[
描述：链接成功
参数：
返回值：无
--]]
local function linkOtherOnConnected(objSession)
    local strIDCard = getID()
    objSession:setIDCard(strIDCard)
end
regGameEvent(GameEvent.LinkedOther, linkOtherOnConnected)
