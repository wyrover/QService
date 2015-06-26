--[[
服务器关闭时处理  
--]]

function onShutDown()
    --确认退出，，，没这个不会退出
    shutDown()
end
regGameEvent(GameEvent.ShutDown, onShutDown)
