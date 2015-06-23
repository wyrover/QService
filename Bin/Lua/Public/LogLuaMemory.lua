--[[
Lua内存使用情况
--]]

local m_iLogMemoTime = 60

local function onLogLuaMemory()
    local iTotalMem = collectgarbage("count")
    local strMemUsage = string.format("lua memory usage:%d,session size:%d", 
        iTotalMem, getSessionSize())
        
    Log(LOGLV_INFO, strMemUsage)
    
    regDelayEvent(m_iLogMemoTime, onLogLuaMemory)
end

local function logLuaMemory()
    regDelayEvent(m_iLogMemoTime, onLogLuaMemory)
end
regGameEvent(GameEvent.Started, logLuaMemory)
