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
function onDelayEvent()   
    local iNow = os.time()
    local tDel = {}
    
    for key, val in pairs(RegFuncs.DelayEvent) do
        if math.abs(iNow - val.RegTime) >= val.Time then
            table.insert(tDel, key)
            
            if val.Func then
                callFunc(val.Func, table.unpack(val.Param))
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
function regDelayEvent(iTime, Func, ...)
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
    
    RegFuncs.DelayEvent[getID()] = tInfo
end

--[[
描述：游戏事件回调
参数：iEvent --事件编号
返回值：无
--]]
function onGameEvent(iEvent, ...)
    if not RegFuncs.GameEvent[iEvent] then
        return
    end
    
    for _, val in pairs(RegFuncs.GameEvent[iEvent]) do     
        if val then
            callFunc(val, table.unpack{...})
        end
    end
end

--[[
描述：游戏事件注册
参数：
返回值：无
--]]
function regGameEvent(iEvent, Func)
    if "function" ~= type(Func) then
        return
    end
    
    if not RegFuncs.GameEvent[iEvent] then
        RegFuncs.GameEvent[iEvent] = {}
    end
    
    table.insert(RegFuncs.GameEvent[iEvent], Func)
end

--[[
描述：网络可读事件回调
参数：
返回值：无
--]]
function onNetEvent(iProtocol, ...)
    local Func = RegFuncs.NetEvent[iProtocol]
    if Func then
        callFunc(Func, table.unpack{...})      
    else
        g_objSessionManager:closeCurLink()
    end
end

--[[
描述：网络可读事件注册
参数：
返回值：无
--]]
function regNetEvent(iProtocol, Func)
    if "function" ~= type(Func) then
        return
    end
    
    RegFuncs.NetEvent[iProtocol] = Func
    Debug("register protocol "..iProtocol)
end
