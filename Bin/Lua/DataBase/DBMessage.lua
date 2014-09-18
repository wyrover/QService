--[[
数据库消息处理
--]]

--[[
描述：日志
参数：
返回值： 无
--]]
local function DBLog(tbMessage)
    DBMgr:insert(DBType_Log, g_LogTableName, tbMessage[ProtocolStr_Info])
end
RegNetEvent(DB_Log, DBLog)

--[[
描述：加载玩家
参数：
返回值： 无
--]]
local function DBLoadPlayer(tbMessage)
    local strAccount = tbMessage[ProtocolStr_Account]
    if (not strAccount) or (0 == string.len(strAccount)) then
        return
    end
    
    tbMessage[ProtocolStr_Info] = {}
    
    local strSql = string.format("SELECT id FROM %s where account = '%s'", 
        g_PlayerTable, strAccount)
    
    local objQuery = DBMgr:executeSql(DBType_Game, strSql)    
    local tCurRow = objQuery:fetch({},"a")
    while tCurRow do
        strSql = string.format("SELECT * FROM %s where id = %s", g_PlayerTable, tCurRow.id)
        
        local objPlayerQuery = DBMgr:executeSql(DBType_Game, strSql)        
        local objPlayerInfo = objPlayerQuery:fetch({},"a")
        while objPlayerInfo do
            table.insert(tbMessage[ProtocolStr_Info], objPlayerInfo)
            objPlayerInfo = objPlayerQuery:fetch(objPlayerInfo, "a")
        end
        
        tCurRow = objQuery:fetch(tCurRow, "a")
    end
    
    local strMsg = cjson.encode(tbMessage)
    g_objSessionManager:sendToCur(strMsg, string.len(strMsg))
    Debug("DBLoadPlayer:"..strMsg)
end
RegNetEvent(DB_LoadPlayer, DBLoadPlayer)

--[[
描述：检查名称是否存在
参数：
返回值： bool
--]]
local function checkNameExist(strName)
    local bHave = false
    local strSql = string.format("SELECT id FROM %s where name = '%s'", 
        g_PlayerTable, strName)
        
    local objQuery = DBMgr:executeSql(DBType_Game, strSql)    
    local tCurRow = objQuery:fetch({},"a")
    if tCurRow then
        bHave = true
    end
    
    return bHave
end

--[[
描述：创建玩家
参数：
返回值： 无
--]]
local function DBCreatePlayer(tbMessage)    
    tbMessage[ProtocolStr_Rtn] = Q_RTN_OK
    
    if not checkNameExist(tbMessage[ProtocolStr_Name]) then
        DBMgr:insert(DBType_Game, g_PlayerTable, tbMessage[ProtocolStr_Info])
    else
        tbMessage[ProtocolStr_Rtn] = GameError_NameRepeat
    end
    
    tbMessage[ProtocolStr_Info] = nil
    
    local strMsg = cjson.encode(tbMessage)
    g_objSessionManager:sendToCur(strMsg, string.len(strMsg))
end
RegNetEvent(DB_CreatPlayer, DBCreatePlayer)

--[[
描述：更新玩家
参数：
返回值： 无
--]]
local function DBSavePlayer(tbMessage)    
    DBMgr:update(DBType_Game, g_PlayerTable, tbMessage[ProtocolStr_Info])
    
    local tRtnMsg = {}
    tRtnMsg[ProtocolStr_Request] = tbMessage[ProtocolStr_Request]
    tRtnMsg[ProtocolStr_CheckID] = tbMessage[ProtocolStr_CheckID]
    
    local strMsg = cjson.encode(tRtnMsg)
    g_objSessionManager:sendToCur(strMsg, string.len(strMsg))
end
RegNetEvent(DB_SavePlayer, DBSavePlayer)
