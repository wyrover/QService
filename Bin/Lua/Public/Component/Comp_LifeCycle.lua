--[[
生命周期组件   
--]]

CompLifeCycle = {}
--继承Component
setmetatable(CompLifeCycle, Component)
CompLifeCycle.__index = CompLifeCycle

--varSerialize 序列化标识 字符串且不为空则序列化为protobuf，table序列化为struct 否则为json
function CompLifeCycle:new(varSerialize)
    local self = Component:new(CompNam_LifeCycle, varSerialize) 
    setmetatable(self, CompLifeCycle)
    
    self.Life = 0--总时间
    self.TVBase = nil--时间轮
    
    return self
end

--接口实现
function CompLifeCycle:getInfo()
    local iLife  = self.Life
    if self.TVBase then
        iLife = self.Life - (getDelayEventTick() - self.TVBase:getRegTick())
    end

    local tInfo = {}
    tInfo.uiLife = iLife

    return tInfo
end

--接口实现
function CompLifeCycle:setInfo(tMsg)
    self.Life = tMsg.uiLife
end

--开始生命周期计时 uiLife 使用self.Life的值
function CompLifeCycle:Register(uiLife, Func, ...)
    if uiLife then
        self.TVBase = regDelayEvent(uiLife, Func, table.unpack({...}))
    else
        self.TVBase = regDelayEvent(self.Life, Func, table.unpack({...}))
    end
end
