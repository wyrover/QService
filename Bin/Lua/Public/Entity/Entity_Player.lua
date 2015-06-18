--[[
玩家  
--]]

Player = {}
--继承Entity
setmetatable(Player, Entity)
Player.__index = Player

--varSerialize 序列化标识 字符串且不为空则序列化为protobuf，table序列化为struct 否则为json
function Player:new(varSerialize)
    local self = Entity:new(varSerialize)
    setmetatable(self, Player)

    --属性组件
    local objAttr = CompAttr:new(varSerialize)
    --设置属性
    objAttr:setAttr(Player_SVID, 0)
    objAttr:setAttr(Player_Account, "")
    objAttr:setAttr(Player_Name, "")

    self:addComponent(objAttr)
    
    return self
end
