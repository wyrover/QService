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
    
    local iDBSessionID = RandOneSV(SVType_DataBase)
    if Q_INVALID_ID == iDBSessionID then
        return
    end
    
    local tLog = {}
    
    tLog[ProtocolStr_Request] = DB_Log
    tLog[ProtocolStr_Log_Type] = LogType
    tLog[ProtocolStr_ServerID] = getServerID()
    
    if PlayerID then
        tLog[ProtocolStr_ID] = PlayerID
    else
        tLog[ProtocolStr_ID] = tostring(Q_INVALID_ID)
    end
    
    if Param0 then
        tLog[ProtocolStr_Log_Param0] = Param0
    else
        tLog[ProtocolStr_Log_Param0] = -1
    end
    
    if Param1 then
        tLog[ProtocolStr_Log_Param1] = Param1
    else
        tLog[ProtocolStr_Log_Param1] = -1
    end
    
    if Param2 then
        tLog[ProtocolStr_Log_Param2] = Param2
    else
        tLog[ProtocolStr_Log_Param2] = -1
    end
    
    if Param3 then
        tLog[ProtocolStr_Log_Param3] = Param3
    else
        tLog[ProtocolStr_Log_Param3] = -1
    end
    
    if Param4 then
        tLog[ProtocolStr_Log_Param4] = Param4
    else
        tLog[ProtocolStr_Log_Param4] = -1
    end
    
    if Description then
        tLog[ProtocolStr_Log_Memo] = Description
    else
        tLog[ProtocolStr_Log_Memo] = ""
    end
    
    local strMsg = cjson.encode(tLog)    
    g_objSessionManager:sendToByID(iDBSessionID, strMsg, string.len(strMsg))
end

