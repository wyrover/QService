--[[
测试
--]]

local function onHttpRead(objHttpBuffer)
    Debug("Query:"..objHttpBuffer:getQuery())
    Debug("PostMsg:"..objHttpBuffer:getPostMsg())
    
    objHttpBuffer:setReplyContent("this is test")
    objHttpBuffer:Reply(HTTP_OK, "OK")
end
regGameEvent(GameEvent.HttpRead, onHttpRead)

local function onStart()
    Debug("onStart")
end
regGameEvent(GameEvent.Start, onStart)

local function onStarted()
    g_StartCompleted = true
    Debug("onStarted")
end
regGameEvent(GameEvent.Started, onStarted)
