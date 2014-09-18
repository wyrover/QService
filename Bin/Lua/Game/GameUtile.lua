--[[
常用函数
--]]

--[[
描述：获取当前连接的玩家
参数：
返回值： 玩家
--]]
function getCurPlayer()
    local objCurSession = g_objSessionManager:getCurSession()
    if not objCurSession then
        return nil
    end
    
    local strPlayerID = objCurSession:getID()
    if 0 == string.len(strPlayerID) then
        return nil
    end
    
    return World:getPlayerMgr():getPlayerByID(strPlayerID)
end

--[[
描述：数据库日志
参数：
返回值： 玩家
--]]
function DBLog(LogType, PlayerID, Param0, Param1, Param2, Param3, Param4, Description)
    if not LogType then
        return
    end
    
    local iDBSessionID = RandOneSV(SVType.DataBase)
    if Q_INVALID_ID == iDBSessionID then
        return
    end
    
    local tLog = {}
    
    tLog[ProtocolStr_Request] = Protocol.DB_Log
    tLog[ProtocolStr_Info] = {}
    
    local tLogAttr = {}
    
    tLogAttr.logtype = LogType
    tLogAttr.serverid = getServerID()
    
    if PlayerID then
        tLogAttr.playerid = PlayerID
    else
        tLogAttr.playerid = tostring(Q_INVALID_ID)
    end
    
    if Param0 then
        tLogAttr.param0 = Param0
    else
        tLogAttr.param0 = -1
    end
    
    if Param1 then
        tLogAttr.param1 = Param1
    else
        tLogAttr.param1 = -1
    end
    
    if Param2 then
        tLogAttr.param2 = Param2
    else
        tLogAttr.param2 = -1
    end
    
    if Param3 then
        tLogAttr.param3 = Param3
    else
        tLogAttr.param3 = -1
    end
    
    if Param4 then
        tLogAttr.param4 = Param4
    else
        tLogAttr.param4 = -1
    end
    
    if Description then
        tLogAttr.description = Description
    else
        tLogAttr.description = ""
    end
    
    tLogAttr.logtime = os.date()
    
    table.insert(tLog[ProtocolStr_Info], tLogAttr)
    
    local strMsg = cjson.encode(tLog)
    g_objSessionManager:sendToByID(iDBSessionID, strMsg, string.len(strMsg))
end
