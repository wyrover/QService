--[[
玩家对象
--]]

Player = {}
Player.__index = Player

--属性列表
g_PlayerAttr = {}

--[[
描述：新建一玩家对象
参数：ID --唯一ID strAccount --账号 strName --名称
返回值： 玩家对象
--]]
function Player:new(ID, strAccount, strName)
        local self = {}        
        setmetatable(self, Player)
        self.ID = ID --唯一ID
        self.strAccount = strAccount --账号
        self.strName = strName --名称
        self.iSessionID = Q_INVALID_ID --session id
        self.bSave = false
        
        return self
end

--[[
描述：设置保存标志
参数：
返回值： 无
--]]
function Player:setSave(bSave)
    self.bSave = bSave
end

--[[
描述：获取保存标志
参数：
返回值： bool
--]]
function Player:getSave()
    return self.bSave
end

--[[
描述：获取玩家ID
参数：
返回值： ID
--]]
function Player:getID()
    return self.ID
end

--[[
描述：获取玩家账号
参数：
返回值： 账号
--]]
function Player:getAccount()
    return self.strAccount
end

--[[
描述：获取玩家名称
参数：
返回值： 名称
--]]
function Player:getName()
    return self.strName
end

--[[
描述：设置玩家对应的session id
参数：iSessionID --session id
返回值:
--]]
function Player:setSessionID(iSessionID)
    self.iSessionID = iSessionID
end

--[[
描述：获取玩家对应的session id
参数：iSessionID --session id
返回值:
--]]
function Player:getSessionID(iSessionID)
    return self.iSessionID
end

--[[
描述：向该玩家发送消息
参数：
返回值:
--]]
function Player:sendMessage(tMessage)
    if not tMessage then
        return
    end
    
    if IsTableEmpty(tMessage) then
        return
    end
    
    if Q_INVALID_ID == self.iSessionID then
        return
    end
    
    local strMsg = cjson.encode(tMessage)
    g_objSessionManager:sendToByID(self.iSessionID, strMsg, string.len(strMsg))
end

--[[
描述：向该玩家发送消息
参数：
返回值:
--]]
function Player:sendStrMessage(strMsg)
    if Q_INVALID_ID == self.iSessionID then
        return
    end
    
    g_objSessionManager:sendToByID(self.iSessionID, strMsg, string.len(strMsg))
end

--[[
描述：返回指定属性的信息
参数：tAttr --要获取的属性 nil或为{}：所有属性
返回值:table{Attr=val,...}
--]]
function Player:getAttr(tAttr)
    local tInfo = {}
    
    tInfo.test1 = 12
    tInfo.test2 = true
    tInfo.test3 = "string"
    tInfo.test4 = {"1", 2, "3", true}
    
    return tInfo
end
