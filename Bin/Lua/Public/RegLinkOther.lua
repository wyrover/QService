--[[
���������������� 
--]]

if not g_LinkedOther then
    g_LinkedOther = {}
end

--[[
�������������������������
������
����ֵ����
--]]
local function reqRegLinkedOther(objSession)
    local tMsg = {}
    tMsg.strName = getServerNam()
    sendToID(objSession:getSessionID(), Protocol.SYS_RegLinkOther, tMsg)
end
regGameEvent(GameEvent.LinkedOther, reqRegLinkedOther)

--[[
����������������������
������
����ֵ����
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
