--[[
组件基类  
--]]

Component = {}
--设置类型是Object    继承
setmetatable(Component, Object)
Component.__index = Component

--strName 组件名称
--varSerialize 序列化标识 字符串且不为空则序列化为protobuf，table序列化为struct 否则为json
function Component:new(strName, varSerialize)
    assert(strName and (0 ~= string.len(strName)))
    
    local self = Object:new(varSerialize)
    setmetatable(self, Component)
    
    self.Name = strName
    
    return self
end

--组件名称
function Component:getName()
    return self.Name
end
