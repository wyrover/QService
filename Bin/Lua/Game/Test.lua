--[[
测试
--]]

local function onDelayEvent(iTime)
    Debug(string.format("onDelayEvent %d...", iTime))
    
    regDelayEvent(iTime + 1, onDelayEvent, iTime + 1)
end
regDelayEvent(3, onDelayEvent, 3)

local function onStart() 
    Debug("onStart...")
end
regGameEvent(GameEvent.Start, onStart)

local function onStarted()
    Debug("onStarted...")
end
regGameEvent(GameEvent.Started, onStarted)

local function onShutDown()
    Debug("onShutDown...")
end
regGameEvent(GameEvent.ShutDown, onShutDown)

local function onOneSecond()
    Debug("onOneSecond...")
end
regGameEvent(GameEvent.OneSecond, onOneSecond)

local function onDayChange()
    Debug("onDayChange...")
end
regGameEvent(GameEvent.DayChange, onDayChange)

local function onWeekChange()
    Debug("onWeekChange...")
end
regGameEvent(GameEvent.WeekChange, onWeekChange)

local function onMonthChange()
    Debug("onMonthChange...")
end
regGameEvent(GameEvent.MonthChange, onMonthChange)

local function onFiveSecond()
    --[[local tInfo = {
        p1 = 11,
        p2 = 11.1,
        p3 = "test"
    }
    local strMsg = protobuf.encode("lm.test", tInfo)
    print(strMsg)
    tInfo = {}
    local tInfo = protobuf.decode("lm.test", strMsg)
    printTable(tInfo)--]]
    
    Debug("onFiveSecond...")
end
regGameEvent(GameEvent.FiveSecond, onFiveSecond)

local function onTenSecond()
    Debug("onTenSecond...")
end
regGameEvent(GameEvent.TenSecond, onTenSecond)

local function onOneMinute()
    Debug("onOneMinute...")
end
regGameEvent(GameEvent.OneMinute, onOneMinute)

local function onFiveMinute()
    Debug("onFiveMinute...")
end
regGameEvent(GameEvent.FiveMinute, onFiveMinute)

local function onTenMinute()
    Debug("onTenMinute...")
end
regGameEvent(GameEvent.TenMinute, onTenMinute)

local function onOneHour()
    Debug("onOneHour...")
end
regGameEvent(GameEvent.OneHour, onOneHour)

local function onClose()
    Debug("onClose...")
end
regGameEvent(GameEvent.Close, onClose)

local function onTestNetEvent(tbMessage)
    Debug("message information:")
    printTable(tbMessage)
    
    local strMsg = ""
    if CarrierType.Json == MsgCarrier then
        strMsg = cjson.encode(tbMessage)
    elseif CarrierType.Protobuf == MsgCarrier then
        local strProro = getProtoStr(1)
        if not strProro then
            Debug("get protobuf string error.")
            return
        end
        
        strMsg = protobuf.encode(strProro, tbMessage)
    else
        Debug("unknown message carrier.")
        return
    end
    
    g_objSessionManager:sendToCur(1, strMsg, string.len(strMsg))
end
regNetEvent(1, onTestNetEvent)
