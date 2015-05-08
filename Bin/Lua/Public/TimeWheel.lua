--[[
时间轮
--]]

--基础属性
TvecBase = {}
TvecBase.__index = TvecBase

function TvecBase:new(Func, tParam, iTimeOut)
    assert("function" == type(Func))
    assert("table" == type(tParam))
    assert("number" == type(iTimeOut))
    
    local self = {}
    setmetatable(self, TvecBase)
    
    self.Func = Func
    self.Param = tParam
    self.TimeOut = iTimeOut
    self.CompTime = 0
	
    return self
end

--下次移动的补偿时间
function TvecBase:setCompTime(iCompTime)
    self.CompTime = iCompTime
end

function TvecBase:getCompTime()
    return self.CompTime
end

--获取超时时间
function TvecBase:getTime()
    return self.TimeOut
end

--超时时间 减
function TvecBase:subTime(iVal)
    self.TimeOut = self.TimeOut - iVal
end

--执行回调函数
function TvecBase:Call()
    callFunc(self.Func, table.unpack(self.Param))
end

--轮
Wheel = {}
Wheel.__index = Wheel

function Wheel:new(iCount, iParam)
    assert("number" == type(iCount))
    
    local self = {}
    setmetatable(self, Wheel)
    
    self.Slot = {}
    for i = 1, iCount do
        table.insert(self.Slot, {})
    end
    
    self.Count = iCount
    self.CurWheel = 0
    self.Param = iParam
    self.Range = iCount * iParam
	
    return self
end

--获取标识范围
function Wheel:getRange()
    return self.Range
end

--获取时间跨度参数
function Wheel:getParam()
    return self.Param
end

--获取轮槽数
function Wheel:getCount()
    return self.Count
end

--增加timer
function Wheel:Add(iIndex, objTvecBase)
    assert(iIndex <= self.Count)
    table.insert(self.Slot[iIndex], objTvecBase)
end

--获取
function Wheel:Get(iIndex)
    assert(iIndex <= self.Count)
    return self.Slot[iIndex]
end

--获取当前槽数
function Wheel:getCurWheel()
    return self.CurWheel
end

--当前槽加1
function Wheel:addCurWheel()
    self.CurWheel = self.CurWheel + 1
end

--重置当前槽
function Wheel:resetCurWheel()
    self.CurWheel = 0
end

--轮管理
WheelMgr = {}
WheelMgr.__index = WheelMgr

function WheelMgr:new()
    local self = {}
    setmetatable(self, WheelMgr)
    self.Sec = Wheel:new(60, 1)--秒
    self.Min = Wheel:new(60, 60)--分
    self.Hor = Wheel:new(24, 60 * 60)--时
    self.Day = Wheel:new(30, 60 * 60 * 24)--天
    self.Mon = Wheel:new(12, 60 * 60 * 24 * 30)--月
    
    return self
end

--获取当前槽与要插入的槽间隔的槽数
function WheelMgr:getCurCompTime(objWheel, iIndex)
    local iCount = 0
    local iCurWheel = objWheel:getCurWheel()
    
    if iIndex > iCurWheel then
        iCount = iIndex - iCurWheel
    else
        iCount = objWheel:getCount() - (iCurWheel - iIndex)
    end
    
    return iCount
end

--获取分补偿时间
function WheelMgr:getMinCompTime(iIndex)
    return self:getCurCompTime(self.Min, iIndex) * self.Min:getParam() - 
        self.Sec:getCurWheel() * self.Sec:getParam()
end

--获取小时补偿时间
function WheelMgr:getHorCompTime(iIndex)
    return self:getCurCompTime(self.Hor, iIndex) * self.Hor:getParam() -
        self.Min:getCurWheel() * self.Min:getParam() -
        self.Sec:getCurWheel() * self.Sec:getParam()
end

--获取天补偿时间
function WheelMgr:getDayCompTime(iIndex)
    return self:getCurCompTime(self.Day, iIndex) * self.Day:getParam() -
        self.Hor:getCurWheel() * self.Hor:getParam() -
        self.Min:getCurWheel() * self.Min:getParam() -
        self.Sec:getCurWheel() * self.Sec:getParam()
end

--获取月补偿时间
function WheelMgr:getMonCompTime(iIndex)
    return self:getCurCompTime(self.Mon, iIndex) * self.Mon:getParam() -
        self.Day:getCurWheel() * self.Day:getParam() -
        self.Hor:getCurWheel() * self.Hor:getParam() -
        self.Min:getCurWheel() * self.Min:getParam() -
        self.Sec:getCurWheel() * self.Sec:getParam()
end

function WheelMgr:Add(iTimeOut, objTvecBase)
    assert(iTimeOut <= self.Mon:getRange())
    --已经到期
    if iTimeOut <= 0 then
        objTvecBase:Call()
        return
    end
    
    local iIndex = 0
    if iTimeOut <= self.Sec:getRange() then
        --秒
        --计算放入位置
        iIndex = ((iTimeOut + self.Sec:getCurWheel() - 1) % self.Sec:getCount()) + 1
        self.Sec:Add(iIndex, objTvecBase)
    elseif iTimeOut <= self.Min:getRange() then
        --分
        --计算多少分
        local iMin = math.floor(iTimeOut / self.Sec:getRange())
        --计算放入位置
        iIndex = ((iMin + self.Min:getCurWheel() - 1) % self.Min:getCount()) + 1
        --设置执行时减去的时间差
        objTvecBase:setCompTime(self:getMinCompTime(iIndex))
        self.Min:Add(iIndex, objTvecBase)
    elseif iTimeOut <= self.Hor:getRange() then
        --小时
        --计算多少小时
        local iHor = math.floor(iTimeOut / self.Min:getRange())
        --计算放入位置
        iIndex = ((iHor + self.Hor:getCurWheel() - 1) % self.Hor:getCount()) + 1
        --设置执行时减去的时间差
        objTvecBase:setCompTime(self:getHorCompTime(iIndex))
        self.Hor:Add(iIndex, objTvecBase)
    elseif iTimeOut <= self.Day:getRange() then
        --天
        --计算多少天
        local iDay = math.floor(iTimeOut / self.Hor:getRange())
        --计算放入位置
        iIndex = ((iDay + self.Day:getCurWheel() - 1) % self.Day:getCount()) + 1
        --设置执行时减去的时间差
        objTvecBase:setCompTime(self:getDayCompTime(iIndex))
        self.Day:Add(iIndex, objTvecBase)
    else
        --月
        --计算多少月
        local iMon = math.floor(iTimeOut / self.Day:getRange())
        --计算放入位置
        iIndex = ((iMon + self.Mon:getCurWheel() - 1) % self.Mon:getCount()) + 1
        --设置执行时减去的时间差
        objTvecBase:setCompTime(self:getMonCompTime(iIndex))
        self.Mon:Add(iIndex, objTvecBase)
    end
end

--增加timer
function WheelMgr:addTimer(Func, iTimeOut, ...)
    local objTvecBase = TvecBase:new(Func, {...}, iTimeOut)
    self:Add(iTimeOut, objTvecBase)
end

--数据移动
function WheelMgr:cashData(objWheel)
    local tRunKey = {}  
    local tWheel = objWheel:Get(objWheel:getCurWheel())    
    for key, _ in pairs(tWheel) do
        table.insert(tRunKey, key)
    end
    
    for i = #tRunKey, 1, -1 do
        tWheel[i]:subTime(tWheel[i]:getCompTime())
        self:Add(tWheel[i]:getTime(), tWheel[i])
        
        table.remove(tWheel, i)
    end
end

--数据移动
function WheelMgr:cashAll()
    local iCurWheel = self.Sec:getCurWheel()
    
    --秒满一圈
    if self.Sec:getCount() == iCurWheel then
        --秒槽数重置
        self.Sec:resetCurWheel()
        
        --分 处理
        self.Min:addCurWheel()
        self:cashData(self.Min)
        
        iCurWheel = self.Min:getCurWheel()
        --分满一圈
        if self.Min:getCount() == iCurWheel then
            --分槽数重置
            self.Min:resetCurWheel()
            
            --小时 处理
            self.Hor:addCurWheel()
            self:cashData(self.Hor)
            
            iCurWheel = self.Hor:getCurWheel()
            --小时满一圈
            if self.Hor:getCount() == iCurWheel then
                --小时槽数重置
                self.Hor:resetCurWheel()
                
                --天 处理
                self.Day:addCurWheel()
                self:cashData(self.Day)
                
                iCurWheel = self.Day:getCurWheel()
                --天满一圈
                if self.Day:getCount() == iCurWheel then
                    --天槽数重置
                    self.Day:resetCurWheel()
                    
                    --月 处理
                    self.Mon:addCurWheel()
                    self:cashData(self.Mon)
                    
                    iCurWheel = self.Mon:getCurWheel()
                    --月满一圈
                    if self.Mon:getCount() == iCurWheel then
                        self.Mon:resetCurWheel()
                    end
                end
            end
        end
    end
end

function WheelMgr:onTime()
    --槽+1
    self.Sec:addCurWheel()
    
    --秒执行当前槽
    local tRunKey = {}    
    local tCurWheel = self.Sec:Get(self.Sec:getCurWheel())    
    for key, _ in pairs(tCurWheel) do
        table.insert(tRunKey, key)
    end
    for i = #tRunKey, 1, -1 do
        --执行
        tCurWheel[i]:Call()
        table.remove(tCurWheel, i)
    end
    
    self:cashAll()
end
