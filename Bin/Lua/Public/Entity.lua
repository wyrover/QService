--[[
实体基类  
--]]

Entity = {}
--继承Object
setmetatable(Entity, Object)
Entity.__index = Entity

--varSerialize 序列化标识 字符串且不为空则序列化为protobuf，table序列化为struct 否则为json
function Entity:new(varSerialize)
    local self = Object:new(varSerialize)
    setmetatable(self, Entity)
    
    self.Component = {}--组件
    
    return self
end

--接口实现
function Entity:getInfo()
    local tInfo = {}
    for key, val in pairs(self.Component) do
        tInfo[key] = val:toString()
    end
    
    return tInfo
end

--接口实现
function Entity:setInfo(tMsg)
    for key, val in pairs(tMsg) do
        self:getComponent(key):fromString(val)
    end
end

--添加组件
function Entity:addComponent(objComp)
    local strName = objComp:getName()
    assert(0 ~= string.len(strName))
    
    self.Component[strName] = objComp
end

--获取组件
function Entity:getComponent(strName)
    return assert(self.Component[strName])
end

--移除组件
function Entity:removeComponent(strName)
    self.Component[strName] = nil 
end
