--[[
服务器注册
--]]

if not g_RegService then
    g_RegService = {}
end

--[[
描述：检查是否注册
参数：
返回值： bool
--]]
function checkSVReg(iSessionID)
    for _, val in pairs(g_RegService) do
        if val == iSessionID then
            return true
        end
    end
    
    return false
end

--[[
描述：服务器注册
参数：
返回值：无
--]]
function RequireRegSV(objSession)
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
描述：注册服务器
参数：
返回值： 无
--]]
local function RegServer(tbMessage)
    local iSVID = tbMessage[ProtocolStr_ServerID]
    if not iSVID then
        return
    end
    
    local objCurSession = g_objSessionManager:getCurSession()
    local iSessionID = objCurSession:getSessionID()
    
    local tRegRtn = {}
    
    tRegRtn[ProtocolStr_Request] = Protocol.System_RegSVRtn
    tRegRtn[ProtocolStr_CheckID] = tbMessage[ProtocolStr_CheckID]
    tRegRtn[ProtocolStr_ClientID] = tbMessage[ProtocolStr_ClientID]
    tRegRtn[ProtocolStr_ServerID] = iSVID
    
    if g_RegService[iSVID] then
        tRegRtn[ProtocolStr_Rtn] = ErrorCode.FAILE
    else
        g_RegService[iSVID] = iSessionID
        tRegRtn[ProtocolStr_Rtn] = ErrorCode.OK
    end
    
    local strMsg = cjson.encode(tRegRtn)
    g_objSessionManager:sendToCur(strMsg, string.len(strMsg))
end
RegNetEvent(Protocol.System_RegSV, RegServer)

--[[
描述：注册服务器返回
参数：
返回值： 无
--]]
local function RegServerRtn(tbMessage)
    local iSVID = tbMessage[ProtocolStr_ServerID]
    local strCheckID = tbMessage[ProtocolStr_CheckID]
    local iClientID = tbMessage[ProtocolStr_ClientID]
    local iRtn = tbMessage[ProtocolStr_Rtn]
    
    local objSession = g_objSessionManager:getSessionByID(iClientID)
    if not objSession then
        return
    end
    
    if objSession:getCheckID() ~= strCheckID then
        return
    end
    
    objSession:setCheckID("")
    if ErrorCode.OK == iRtn then
        Debug("register service ".. tonumber(iSVID) .. " successfully.")
        return
    end
    
    Debug("register service ".. tonumber(iSVID) .. " error.")
    CloseLink(iClientID)
end
RegNetEvent(Protocol.System_RegSVRtn, RegServerRtn)

--[[
描述：连接断开时
参数：
返回值： 无
--]]
local function OnRegSVClose()
    local objCurSession = g_objSessionManager:getCurSession()
    local iSessionID = objCurSession:getSessionID()
    
    for key, val in pairs(g_RegService) do
        if val == iSessionID then
            g_RegService[key] = nil
            Debug("remove register server "..tonumber(key))
            break
        end
    end
end
RegGameEvent(GameEvent.Close, "OnRegSVClose", OnRegSVClose)

--[[
描述：移除服务器注册,用于强制移除
参数：
返回值： 无
--]]
function RemoveRegSV(iSVID)
    if not g_RegService[iSVID] then
        return
    end
    
    local iSessionID = g_RegService[iSVID]
    CloseLink(iSessionID)
    g_RegService[iSVID] = nil
end
