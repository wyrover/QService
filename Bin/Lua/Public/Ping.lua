--[[
ping
--]]

--ping 超时时间(秒)
local m_iPingTimeOut = 5 * 60
local m_iSendPintTime = 2 * 60

--[[
描述：ping
参数：
返回值：无
--]]
local function CSPing(tbMessage)
    local objCurSession = getCurSession()    
    objCurSession:setPing(g_objSessionMgr:getCount())
    
    sendToCur(Protocol.SC_Ping)
end
regNetEvent(Protocol.CS_Ping, CSPing)

--[[
描述：ping返回
参数：
返回值：无
--]]
local function SCPing(tbMessage)
    local objCurSession = getCurSession()    
    objCurSession:setPing(g_objSessionMgr:getCount())
end
regNetEvent(Protocol.SC_Ping, SCPing)

--[[
描述：ping检查
参数：
返回值：无
--]]
local function pingCheck(strIDCard, iSessionID)
    local objSession = getSessionByID(iSessionID)
    if not objSession then
        return
    end
    
    if strIDCard ~= objSession:getIDCard() then
        return
    end
    
    local iPassTime = (g_objSessionMgr:getCount() - objSession:getPing()) * g_objSessionMgr:getTimer()
    iPassTime = math.floor(iPassTime / 1000)
    if iPassTime >= m_iPingTimeOut then
        Debug("ID Card %s, session id %d timeout.", strIDCard, iSessionID)
        closeID(iSessionID)
        
        return
    end
    
    regDelayEvent(m_iPingTimeOut, pingCheck, strIDCard, iSessionID)
end

--[[
描述：ping检查注册
参数：
返回值：无
--]]
local function pingOnConnected(objSession)
    local strIDCard = getID()
    objSession:setIDCard(strIDCard)
    
    regDelayEvent(m_iPingTimeOut, pingCheck, strIDCard, objSession:getSessionID())
end
regGameEvent(GameEvent.OnConnected, pingOnConnected)

--[[
描述：ping发送
参数：
返回值：无
--]]
local function pingSend(strIDCard, iSessionID)
    local objSession = getSessionByID(iSessionID)
    if not objSession then
        return
    end
    
    if strIDCard ~= objSession:getIDCard() then
        return
    end
    
    sendToID(iSessionID, Protocol.CS_Ping)
    
    regDelayEvent(m_iSendPintTime, pingSend, strIDCard, iSessionID)
end

--[[
描述：ping检查注册
参数：
返回值：无
--]]
local function pingOnLinkedOther(objSession)
    local strIDCard = getID()
    objSession:setIDCard(strIDCard)
    
    regDelayEvent(m_iPingTimeOut, pingCheck, strIDCard, objSession:getSessionID())
    regDelayEvent(m_iSendPintTime, pingSend, strIDCard, objSession:getSessionID())
end
regGameEvent(GameEvent.LinkedOther, pingOnLinkedOther)
