--[[
���ӳɹ�ִ��
--]]

--[[
���������ӳɹ�
������
����ֵ����
--]]
local function clientOnConnected(objSession)
    local strIDCard = getID()
    objSession:setIDCard(strIDCard)
end
regGameEvent(GameEvent.OnConnected, clientOnConnected)

--[[
���������ӳɹ�
������
����ֵ����
--]]
local function linkOtherOnConnected(objSession)
    local strIDCard = getID()
    objSession:setIDCard(strIDCard)
end
regGameEvent(GameEvent.LinkedOther, linkOtherOnConnected)
