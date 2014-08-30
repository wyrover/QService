--[[
玩家对象
--]]

Player = {}
Player.__index = Player

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
        
        return self
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


--事件注册
local function OnPlayerEnterGame(objPlayer)
    --add your code
end
RegGameEvent(GameEvent_LogIned, "OnPlayerEnterGame", OnPlayerEnterGame)

local function OnPlayerLogOut()
    local objPlayer = getCurPlayer()
    if not objPlayer then
        return
    end
    
    --add your code
    
    World:getPlayerMgr():setOnLineStatus(objPlayer:getID(), false)    
    objPlayer:setSessionID(Q_INVALID_ID)
    
    DBLog(DBLogType_LogOut, objPlayer:getID())    
    Debug(string.format("player logout,id %s, name %s", objPlayer:getID(), objPlayer:getName()))
end
RegGameEvent(GameEvent_Close, "OnPlayerLogOut", OnPlayerLogOut)
