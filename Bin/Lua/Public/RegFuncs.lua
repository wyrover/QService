--[[
函数注册
--]]

local m_iOneDay = 24 * 60 * 60

if not RegFuncs then
    RegFuncs = {}
    
    --网络消息
    RegFuncs.NetEvent = {}
    --proto消息字符串
    RegFuncs.ProtoStr = {}
    --游戏事件
    RegFuncs.GameEvent = {}
    --延迟事件
    RegFuncs.DelayEvent = WheelMgr:new()
end

--proto消息字符串
function setProtoStr(iProtocol, strProto)
    if strProto and (0 ~= string.len(strProto)) then
        RegFuncs.ProtoStr[iProtocol] = strProto
    end
end
function getProtoStr(iProtocol)
    local strProto = RegFuncs.ProtoStr[iProtocol]
    assert(strProto, string.format("get protobuf type by protocol %d error.", iProtocol))
    
    return strProto
end

--[[
描述：延迟事件执行
参数：
返回值：无
--]]
function onDelayEvent()   
    RegFuncs.DelayEvent:onTime()
end

--[[
描述：延迟事件注册
参数：iTime --延后时间（秒）
返回值：TvecBase
--]]
function regDelayEvent(iTime, Func, ...)
    return RegFuncs.DelayEvent:addTimer(Func, iTime, table.unpack({...}))
end
function regDelayEventByBase(objTvecBase)
    return RegFuncs.DelayEvent:Add(objTvecBase:getTime(), objTvecBase)
end
--strTime 格式(24小时制)：12:36:28
function regDelayEventAtTime(strTime, Func, ...)
    local strHour, strMin, strSec = string.match(strTime, "(%d+):(%d+):(%d+)")
    local iTime = (tonumber(strHour) * 60 * 60) + (tonumber(strMin) * 60) + tonumber(strSec)
    local tNow = os.date("*t", time)
    local iNowTime = (tonumber(tNow.hour) * 60 * 60) + (tonumber(tNow.min) * 60) + tonumber(tNow.sec)
    
    local iDelayTime = 0
    
    if iTime >= iNowTime then
        iDelayTime = iTime - iNowTime
    else
        iDelayTime = m_iOneDay - (iNowTime - iTime)
    end
    
    return regDelayEvent(iDelayTime, Func, table.unpack({...}))
end

--[[
描述：延迟事件移除
参数：objTvecBase
返回值：无
--]]
function removDelayEvent(objTvecBase)
    RegFuncs.DelayEvent:Remove(objTvecBase)
end

--[[
描述：延迟事件总共走过多少时间
参数：无
返回值：无
--]]
function getDelayEventTick()
    return RegFuncs.DelayEvent:getTick()
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
    Debug("register protocol %d", iProtocol)
end
