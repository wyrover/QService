--[[
常用函数
--]]

--[[
描述：session获取
参数：
返回值：无
--]]
function getCurSession()
    return g_objSessionMgr:getCurSession()
end

function getSessionByID(iSessionID)
    return g_objSessionMgr:getSessionByID(iSessionID)
end

--[[
描述：发送
参数：
返回值：无
--]]
function sendToCur(iProtocol, tMsg)
    local objCur = getCurSession()
    if not objCur then
        return
    end
    
    createMsg(iProtocol, tMsg)
    g_objSessionMgr:sendBToCur()
end

function sendToID(iSessionID, iProtocol, tMsg)
    if Q_INVALID_SOCK == iSessionID then
        return
    end
    
    createMsg(iProtocol, tMsg)
    g_objSessionMgr:sendBToByID(iSessionID)
end

--向链接到服务器的链接发送消息
function sendToName(strName, iProtocol, tMsg)
    local iSessionID = g_objSessionMgr:getLinkOtherID(strName)
    if Q_INVALID_SOCK == iSessionID then
        return
    end
    
    sendToID(iSessionID, iProtocol, tMsg)
end

--向其他服务器的链接发送消息
function returnToName(strName, iProtocol, tMsg)
    for _, val in pairs(g_LinkedOther) do
        if strName == val[1] then
            sendToID(val[2], iProtocol, tMsg)
        end
    end
end

--[[
描述：连接关闭
参数：
返回值： 无
--]]
local function realClose(iSessionID, strIDCard)
    local objSession = getSessionByID(iSessionID)
    if objSession:getIDCard() ~= strIDCard then
        return
    end
    
    g_objSessionMgr:closeLinkByID(iSessionID)
end

function closeCur()
    local objSession = getCurSession()
    if not objSession then
        return
    end
    
    local iSessionID = objSession:getSessionID()
    local strIDCard = objSession:getIDCard()
    
    sendToCur(Protocol.SC_Close)
    
    regDelayEvent(5, realClose, iSessionID, strIDCard)
end

function closeID(iSessionID)
    local objSession = getSessionByID(iSessionID)
    if not objSession then
        return
    end
    
    local iSessionID = objSession:getSessionID()
    local strIDCard = objSession:getIDCard()
    
    sendToID(iSessionID, Protocol.SC_Close)
    
    regDelayEvent(5, realClose, iSessionID, strIDCard)
end

--[[
描述：退出服务器
参数：
返回值： 无
--]]
function shutDown()
    g_objSessionMgr:confirmStop()
end

--[[
描述：添加tcp连接
参数：
返回值： 无
--]]
function addLinker(strIp, usPort, strName, usType)
    assert(strIp and usPort and strName and usType)

    g_objSessionMgr:addLinkOther(strIp, usPort, strName, usType)
end

--[[
描述：sesion数量
参数：
返回值： 无
--]]
function getSessionSize()
    return g_objSessionMgr:getSessionSize()
end

