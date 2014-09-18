--[[
玩家对象管理
--]]

PlayerManager = {}
PlayerManager.__index = PlayerManager

--[[
描述：新建一玩家管理对象
参数：
返回值： 玩家管理对象
--]]
function PlayerManager:new()
        local self = {}        
        setmetatable(self, PlayerManager)
        self.Player = {} --存储玩家
        self.MapAccountID = {} --存储玩家账号、ID对应信息
        self.MapNameID = {} --存储玩家名称、ID对应信息
        self.OnLine = {} --储存在线玩家
        self.SavePlayerCheckID = {}--验证ID
        
        return self
end

--[[
描述: 添加一玩家
参数:
返回值:
--]]
function PlayerManager:addPlayer(objPlayer)
    local ID = objPlayer:getID()
    local strAccount = objPlayer:getAccount()
    local strName = objPlayer:getName()
    
    --存储玩家
    self.Player[ID] = objPlayer
    
    --玩家名称ID对应
    self.MapNameID[strName] = ID
    
    --玩家账号ID对应
    if not self.MapAccountID[strAccount] then
        self.MapAccountID[strAccount] = {}
    end
    
    --是否已经存在
    for _, val in pairs(self.MapAccountID[strAccount]) do
        if val == ID then
            return
        end
    end
    
    table.insert(self.MapAccountID[strAccount], ID)
end

--[[
描述: 根据ID获取玩家
参数: 
返回值: 玩家信息
--]]
function PlayerManager:getPlayerByID(ID)
    return self.Player[ID]
end

--[[
描述: 根据名称获取玩家
参数: 
返回值: 玩家信息
--]]
function PlayerManager:getPlayerByName(strName)
    local ID = self.MapNameID[strName]
    if not ID then
        return nil
    end
    
    return self:getPlayerByID(ID)
end

--[[
描述: 根据账号获取玩家
参数: 
返回值: 玩家信息(table)
--]]
function PlayerManager:getPlayerByAccount(strAccount)
    if not self.MapAccountID[strAccount] then
        return nil
    end
    
    local tRtn = {}
    for _, val in pairs(self.MapAccountID[strAccount]) do
        local objPlayer = self:getPlayerByID(val)
        if objPlayer then
            table.insert(tRtn, objPlayer)
        end
    end
    
    if IsTableEmpty(tRtn) then
        return nil
    end
    
    return tRtn
end

--[[
描述: 设置玩家在线状态
参数: 
返回值: 
--]]
function PlayerManager:setOnLineStatus(ID, bOnLine)
    if bOnLine then
        self.OnLine[ID] = true
    else
        self.OnLine[ID] = nil
    end
end

--[[
描述: 检查玩家在线状态
参数: 
返回值: bool
--]]
function PlayerManager:checkOnLineStatus(ID)
    if self.OnLine[ID] then
        return true
    else
        return false
    end
end

--[[
描述: 获取在线玩家ID
参数: 
返回值: table
--]]
function PlayerManager:getOnLinePlayerID()
    local tRtn = {}
    
    for key,_ in pairs(self.OnLine) do
        table.insert(tRtn, key)
    end
    
    return tRtn
end

--[[
描述: 获取在线玩家数
参数: 
返回值: int
--]]
function PlayerManager:getOnLineNum()
    return TableLens(self.OnLine)
end

--[[
描述: 获取账号下角色数量
参数: 
返回值: int
--]]
function PlayerManager:getRoleNum(strAccount)
    if not self.MapAccountID[strAccount] then
        return 0
    end
    
    return TableLens(self.MapAccountID[strAccount])
end

--[[
描述: 移除保存验证码
参数: 
返回值: 无
--]]
function PlayerManager:removSaveCheckID(strCheckID)
    for key, val in pairs(self.SavePlayerCheckID) do
        if val == strCheckID then
            table.remove(self.SavePlayerCheckID, key)
            return
        end
    end
end

--[[
描述: 保存验证码是否为空
参数: 
返回值: 无
--]]
function PlayerManager:saveCheckIDEmpty(strCheckID)
    return IsTableEmpty(self.SavePlayerCheckID)
end

--[[
描述: 保存验证码清空
参数: 
返回值: 无
--]]
function PlayerManager:saveCheckIDEmpty(strCheckID)
    self.SavePlayerCheckID = {}
end

--[[
描述: 广播
参数: 
返回值: 无
--]]
function PlayerManager:broadCast(tbMessage)
    local objPlayer = nil
    
    local strMsg = cjson.encode(tbMessage)
    
    for key, _ in pairs(self.OnLine) do
        objPlayer = self.getPlayerByID(key)
        if objPlayer then
            objPlayer:sendStrMessage(strMsg)
        end
    end
end

--[[
描述: 保存
参数: strPlayerID nill:所有玩家 tAttr --nil或为{} 为所有属性
返回值: 无
--]]
function PlayerManager:Save(strPlayerID, tAttr)
    local iDBSessionID = RandOneSV(SVType.DataBase)    
    if Q_INVALID_ID == iDBSessionID then
        Debug("save player error,not linked to any database server")
        Q_LOG(LOGLV_ERROR, "save player error,not linked to any database server") 
        
        return
    end
    
    --是否为保存所有属性
    local bSaveAll = false
    if (not tAttr) or (IsTableEmpty(tAttr)) then
        bSaveAll = true
    end
    
    local tSaveMsg = {}
    local strCheckID = nil
    local strMsg = nil
    
    tSaveMsg[ProtocolStr_Request] = Protocol.DB_SavePlayer
    tSaveMsg[ProtocolStr_Info] = {}
    
    --保存单个
    if strPlayerID then
        if 0 == string.len(strPlayerID) then
            return
        end
        
        local objPlayer = self.getPlayerByID(strPlayerID)
        if not objPlayer then
            return
        end
        
        local Info = objPlayer:getAttr(tAttr)
        if (not Info) or (IsTableEmpty(Info)) then
            return
        end
        
        strCheckID = GetID()
        tSaveMsg[ProtocolStr_CheckID] = strCheckID
        tSaveMsg[ProtocolStr_Info][strPlayerID] = Info
        
        table.insert(self.SavePlayerCheckID, strCheckID)
        
        strMsg = cjson.encode(tSaveMsg)        
        g_objSessionManager:sendToByID(iDBSessionID, strMsg, string.len(strMsg))
        
        if not self:checkOnLineStatus(strPlayerID) then
            if bSaveAll then
                objPlayer:setSave(false)
            end
        end
        
        return
    end
    
    --保存多个
    local iCount = 0
    for key, val in pairs(self.Player) do
        if val:getSave() then
            local Info = val:getAttr(tAttr)
            if (Info) and (not IsTableEmpty(Info)) then
                iCount = iCount + 1                
               
                tSaveMsg[ProtocolStr_Info][key] = Info
                if not self:checkOnLineStatus(key) then
                    if bSaveAll then
                        self.Player[key]:setSave(false)
                    end
                end
                
                --每SavePlayer_PreNum个玩家请求次保存
                if iCount >= SavePlayer_PreNum then
                    strCheckID = GetID()
                    tSaveMsg[ProtocolStr_CheckID] = strCheckID
                    table.insert(self.SavePlayerCheckID, strCheckID)
        
                    strMsg = cjson.encode(tSaveMsg)
                    g_objSessionManager:sendToByID(iDBSessionID, strMsg, string.len(strMsg))
                        
                    iCount = 0
                    tSaveMsg[ProtocolStr_Info] = {}
                end
            else
                Debug("get player"..key.." attribute error.")
            end
        else
            Debug("player "..key.." not need save")
        end
    end
    
    if iCount > 0 then
        strCheckID = GetID()
        tSaveMsg[ProtocolStr_CheckID] = strCheckID
        table.insert(self.SavePlayerCheckID, strCheckID)
        
        strMsg = cjson.encode(tSaveMsg)
        g_objSessionManager:sendToByID(iDBSessionID, strMsg, string.len(strMsg))
    end
end
