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
描述：检查玩家表字段是否存在
参数：
返回值： bool
--]]
local function checkPlayerField(strField, tNewFiled)
    for _, val in pairs(g_PlayerField) do
        if string.lower(val) == string.lower(strField) then
            return true
        end
    end
    
    for _,val in pairs(tNewFiled) do
        if string.lower(val) == string.lower(strField) then
            return true
        end
    end
    
    return false
end

--[[
描述：更新玩家
参数：
返回值： 无
--]]
local function DBSavePlayer(tbMessage)
    local objLinker = DBMgr:getLinker(DBType_Game)
    if not objLinker then
        return
    end
    
    local tInfo = tbMessage[ProtocolStr_Info]
    if IsTableEmpty(tInfo) then
        return
    end
    
    local strSql = nil
    local strAddFiledSql = nil
    local tNewFiled = {}
    
    objLinker:Transaction()
    for strPlayerID, tAttrInfo in pairs(tInfo) do
        --构造更新sql
        strSql = "UPDATE player SET"
        for strAttr, AttrInfo in pairs(tAttrInfo) do 
            --字段检查
            local strType = type(AttrInfo)
            if not checkPlayerField(strAttr, tNewFiled) then
                if ("number" == strType) then
                    strAddFiledSql = string.format("ALTER TABLE player ADD COLUMN `%s` INTEGER UNSIGNED NOT NULL DEFAULT 0", strAttr)
                elseif ("boolean" == strType) then
                    strAddFiledSql = string.format("ALTER TABLE player ADD COLUMN `%s` CHAR(1) NOT NULL DEFAULT 0", strAttr)
                elseif("string" == strType) or ("table" == strType) then
                    strAddFiledSql = string.format("ALTER TABLE player ADD COLUMN `%s` TEXT NOT NULL", strAttr)
                else
                    Debug("unknown field type")
                    objLinker:rollBack()                   
                    return
                end
                
                objLinker:execDML(strAddFiledSql)
                table.insert(tNewFiled, strAttr)
            end
            
            --根据类型构造sql
            if ("number" == strType) then
                strSql = string.format("%s %s=%d,", strSql, strAttr, AttrInfo)
            elseif("boolean" == strType) then
                if AttrInfo then
                    strSql = string.format("%s %s=%d,", strSql, strAttr, 1)
                else
                    strSql = string.format("%s %s=%d,", strSql, strAttr, 0)
                end                
            elseif("string" == strType) then 
                strSql = string.format("%s %s='%s',", strSql, strAttr, AttrInfo)
            elseif("table" == strType) then
                strSql = string.format("%s %s='%s',", strSql, strAttr, cjson.encode(AttrInfo))
            else
                Debug("unknown field type")
                objLinker:rollBack()                   
                return
            end
        end
        
        --去掉最后个,号
        strSql = string.sub(strSql, 1, string.len(strSql) - 1)
        strSql = string.format("%s WHERE id=%s", strSql, strPlayerID)
        
        objLinker:execDML(strSql)
    end
    objLinker:commitTransaction()
    
    for _, val in pairs(tNewFiled) do
        table.insert(g_PlayerField, val)
    end
    
    local tRtnMsg = {}
    tRtnMsg[ProtocolStr_Request] = tbMessage[ProtocolStr_Request]
    tRtnMsg[ProtocolStr_CheckID] = tbMessage[ProtocolStr_CheckID]
    
    local strMsg = cjson.encode(tRtnMsg)
    g_objSessionManager:sendToCur(strMsg, string.len(strMsg))
end
RegNetEvent(DB_SavePlayer, DBSavePlayer)
