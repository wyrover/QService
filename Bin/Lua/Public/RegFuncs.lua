--[[
函数注册
--]]

if not RegFuncs then
    RegFuncs = {}
    
    --网络消息
    RegFuncs.NetEvent = {}
    --游戏事件
    RegFuncs.GameEvent = {}
    --延迟事件
    RegFuncs.DelayEvent = {}
end

--[[
描述：延迟事件回调,执行后便删除
参数：
返回值：无
--]]
function OnDelayEvent()   
    local iNow = os.time()
    local tDel = {}
    
    for key, val in pairs(RegFuncs.DelayEvent) do
        if  math.abs(iNow - val.RegTime) >= val.Time then
            table.insert(tDel, key)
            
            local Func = val.Func
            if Func then
                CallFunc(Func, table.unpack(val.Param))
            end
        end
    end
    
    for _, val in pairs(tDel) do
        RegFuncs.DelayEvent[val] = nil
    end
end

--[[
描述：延迟事件注册
参数：iTime --延后时间（秒）
返回值：无
--]]
function RegDelayEvent(iTime, Func, ...)
    if "function" ~= type(Func) then
        return
    end
    
    if 0 >= iTime then
        return
    end
    
    local tInfo = {}
    
    tInfo.RegTime = os.time()
    tInfo.Time = iTime
    tInfo.Func = Func
    tInfo.Param = {...}
    
    RegFuncs.DelayEvent[GetID()] = tInfo
end

--[[
描述：游戏事件回调
参数：iEvent --事件编号
返回值：无
--]]
function OnGameEvent(iEvent, ...)
    if not RegFuncs.GameEvent[iEvent] then
        return
    end
    
    for _, val in pairs(RegFuncs.GameEvent[iEvent]) do
        local strFunc = val[1]
        local Func = val[2]
        
        if Func then
            TimerReStart()
            CallFunc(Func, table.unpack{...})
            Debug("function "..strFunc.." elapsed time:"..tostring(TimerElapsed()).. " ms")
        end
    end
end

--[[
描述：游戏事件注册
参数：
返回值：无
--]]
function RegGameEvent(iEvent, strFunc, Func)
    if "function" ~= type(Func) then
        return
    end
    
    if not strFunc then
        return
    end
    
    if not RegFuncs.GameEvent[iEvent] then
        RegFuncs.GameEvent[iEvent] = {}
    end
    
    for _, val in pairs(RegFuncs.GameEvent[iEvent]) do
        if val[1] == strFunc then
            return
        end
    end
    
    table.insert(RegFuncs.GameEvent[iEvent], {strFunc, Func})
end

--[[
描述：网络可读事件回调
参数：
返回值：无
--]]
function OnNetEvent(iOpCode, tbMessage)
    local Func = RegFuncs.NetEvent[iOpCode]
    if Func then
        TimerReStart()
        CallFunc(Func, tbMessage)
        Debug("protocol "..iOpCode.." elapsed time:"..tostring(TimerElapsed()).. " ms")        
    else
        Debug("unknown protocol " .. iOpCode ..", close this link.")
        g_objSessionManager:closeCurLink()
    end
end

--[[
描述：网络可读事件注册
参数：
返回值：无
--]]
function RegNetEvent(iOpCode, Func)
    if "function" ~= type(Func) then
        return
    end
    
    RegFuncs.NetEvent[iOpCode] = Func
    Debug("register protocol "..iOpCode)
end
