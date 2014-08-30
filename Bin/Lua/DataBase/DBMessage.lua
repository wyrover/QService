--[[
数据库消息处理
--]]

--[[
描述：日志
参数：
返回值： 无
--]]
local function DBLog(tbMessage)
    local objLinker = DBMgr:getLinker(DBType_Log)
    if not objLinker then
        return
    end
    
    local strSql = string.format("INSERT INTO %s \
        (serverid, playerid, logtype, logtime, param0, param1, param2, param3, param4, description)\
        values(%d, %s, %d, '%s', %d, %d, %d, %d, %d, '%s')", 
        g_LogTableName,
        tbMessage[ProtocolStr_ServerID],
        tbMessage[ProtocolStr_ID],
        tbMessage[ProtocolStr_Log_Type],
        os.date(),
        tbMessage[ProtocolStr_Log_Param0],
        tbMessage[ProtocolStr_Log_Param1],
        tbMessage[ProtocolStr_Log_Param2],
        tbMessage[ProtocolStr_Log_Param3],
        tbMessage[ProtocolStr_Log_Param4],
        tbMessage[ProtocolStr_Log_Memo])
        
    objLinker:execDML(strSql)
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
		
	local strSql = string.format("SELECT id,name FROM player where account = '%s'", 
		strAccount)
	tbMessage[ProtocolStr_Info] = {}
	local objQuery = objLinker:execQuery(strSql)
	while not objQuery:eof() do
		local  tInfo = {}
		--读取玩家信息
		tInfo[ProtocolStr_ID] = objQuery:getStrInt64Field("id", 0)
		tInfo[ProtocolStr_Name] = objQuery:getStringField("name", "")

		table.insert(tbMessage[ProtocolStr_Info], tInfo)
		
		objQuery:nextRow()
	end
    
	objQuery:finalize()
	
	local strMsg = cjson.encode(tbMessage)
    g_objSessionManager:sendToCur(strMsg, string.len(strMsg))
end
RegNetEvent(DB_LoadPlayer, DBLoadPlayer)

--[[
描述：检查名称是否存在
参数：
返回值： bool
--]]
local function checkNameExist(objLinker, strName)
    local bHave = false
    local strSql = string.format("SELECT id FROM player where name = '%s'", 
		strName)
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
        local strSql = string.format("insert into player (id, account, name) values (%s, '%s', '%s')",
            tbMessage[ProtocolStr_ID], tbMessage[ProtocolStr_Account], tbMessage[ProtocolStr_Name])
        
        local iRtn = objLinker:execDML(strSql)
        if 0 == iRtn then
            tbMessage[ProtocolStr_Rtn] = Q_RTN_FAILE
        end
    else
        tbMessage[ProtocolStr_Rtn] = GameError_NameRepeat
    end
    
    local strMsg = cjson.encode(tbMessage)
    g_objSessionManager:sendToCur(strMsg, string.len(strMsg))
end
RegNetEvent(DB_CreatPlayer, DBCreatePlayer)

--[[
描述：更新玩家
参数：
返回值： 无
--]]
