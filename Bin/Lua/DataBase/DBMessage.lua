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

    local objLinker = DBMgr:getLinker(DBType_Game)
    if not objLinker then
        return
    end
    
    tbMessage[ProtocolStr_Info] = {}
    
    local strSql = string.format("SELECT id FROM %s where account = '%s'", 
        g_PlayerTable, strAccount)    
    local objQuery = objLinker:execQuery(strSql)
    while not objQuery:eof() do
        local strID = objQuery:getStrInt64Field("id", 0)
        local tInfo = DBMgr:loadInfo(DBType_Game, g_PlayerTable, strID)
        if tInfo then
            for _, val in pairs(tInfo) do
                table.insert(tbMessage[ProtocolStr_Info], val)
            end
        end
        
        objQuery:nextRow()
    end
    
    objQuery:finalize()
    
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
local function checkNameExist(objLinker, strName)
    local bHave = false
    local strSql = string.format("SELECT id FROM %s where name = '%s'", 
        g_PlayerTable, strName)
    local objQuery = objLinker:execQuery(strSql)
    if not objQuery:eof() then        
        bHave = true
    end
    
    objQuery:finalize()
    
    return bHave
end

--[[
描述：创建玩家
参数：
返回值： 无
--]]
local function DBCreatePlayer(tbMessage)
    local objLinker = DBMgr:getLinker(DBType_Game)
    if not objLinker then
        return
    end
    
    tbMessage[ProtocolStr_Rtn] = Q_RTN_OK    
    if not checkNameExist(objLinker, tbMessage[ProtocolStr_Name]) then
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
