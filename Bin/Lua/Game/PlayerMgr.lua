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
