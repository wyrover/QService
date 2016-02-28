--[[
服务器间链接设置 
--]]

if not g_LinkedOther then
    g_LinkedOther = {}
end

--[[
描述：请求服务器间链接设置
参数：
返回值：无
--]]
local function reqRegLinkedOther(objSession)
    local tMsg = {}
    tMsg.strName = getServerNam()
    sendToID(objSession:getSessionID(), Protocol.SYS_RegLinkOther, tMsg)
end
regGameEvent(GameEvent.LinkedOther, reqRegLinkedOther)

--[[
描述：服务器间链接设置
参数：
返回值：无
--]]
local function regLinkedOther(tbMessage)
    local objCurSession = getCurSession()
    g_LinkedOther[objCurSession:getIDCard()] = {tbMessage.strName, objCurSession:getSessionID()}
end
regNetEvent(Protocol.SYS_RegLinkOther, regLinkedOther)

local function removeLinkedOther(objSession)
    g_LinkedOther[objSession:getIDCard()] = nil
end
regGameEvent(GameEvent.Close, removeLinkedOther)
