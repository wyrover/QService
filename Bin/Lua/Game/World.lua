--[[
整个游戏管理器
--]]

GameWorld = {}
GameWorld.__index = GameWorld

--[[
描述：新建一游戏管理器
参数：
返回值： 游戏管理器
--]]
function GameWorld:new()
        local self = {}        
        setmetatable(self, GameWorld)
        self.PlayerMgr = PlayerManager:new()
        
        return self
end

--[[
描述：获取玩家管理器
参数：
返回值： 玩家管理器
--]]
function GameWorld:getPlayerMgr()
    return self.PlayerMgr
end

--[[
描述：游戏初始化
参数：
返回值:
--]]
function GameWorld:initWorld()
    Debug("begin init game world.")
    Debug("end init game world.")
end

--[[
描述：游戏关闭
参数：
返回值:
--]]
function GameWorld:shutDown()
    Debug("begin shutdown game world.")
    Debug("end shutdown game world.")
end

--创建一游戏世界
if not World then
    World = GameWorld:new()
end

--事件注册
local function InitWorld()
    World:initWorld()
end
RegGameEvent(GameEvent_StartUp, "InitWorld", InitWorld)

local function ShutDownWorld()
    World:shutDown()
end
RegGameEvent(GameEvent_ShutDown, "ShutDownWorld", ShutDownWorld)

local function On10Minute()
    World:getPlayerMgr():Save(nil, nil)
end
RegGameEvent(GameEvent_1Minute, "On10Minute", On10Minute)

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
