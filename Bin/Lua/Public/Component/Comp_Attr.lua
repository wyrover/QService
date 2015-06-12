--[[
属性组件   
--]]

CompAttr = {}
--设置类型是Component    继承
setmetatable(CompAttr, Component)
CompAttr.__index = CompAttr

--varSerialize 序列化标识 字符串且不为空则序列化为protobuf，table序列化为struct 否则为json
function CompAttr:new(varSerialize)
    local self = Component:new(CompNam_Attr, varSerialize) 
    setmetatable(self, CompAttr)
    
    self.Attr = {}
    
    return self
end

--接口实现
function CompAttr:getInfo()
    return self.Attr
end

--接口实现
function CompAttr:setInfo(tMsg)
    self.Attr = tMsg
end

--设置属性
function CompAttr:setAttr(varKey, varVal)
    self.Attr[varKey] = varVal
end

--属性获取
function CompAttr:getAttr(varKey)   
    return assert(self.Attr[varKey])
end

--属性增加
function CompAttr:addAttr(varKey, iVal)
    assert("number" == type(iVal))
    assert(self.Attr[varKey])
    
    self.Attr[varKey] = self.Attr[varKey] + iVal
end
