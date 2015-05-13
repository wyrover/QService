--[[
测试
--]]

--[[require ("mysql")
local objMysqlENV = assert(mysql.mysql())
local objConn = assert(objMysqlENV:connect("game", "root", "123456", "127.0.0.1", 3306))
--]]

local function onShutDown()
    --[[objConn:execute("delete from test")
    objConn:close()
    objMysqlENV:close()--]]
    Debug("onShutDown")
end
regGameEvent(GameEvent.ShutDown, onShutDown)

local function onHttpRead(objHttpBuffer)
    Debug("Query:"..objHttpBuffer:getQuery())
    Debug("PostMsg:"..objHttpBuffer:getPostMsg())
    
    objHttpBuffer:setReplyContent("this is test")
    objHttpBuffer:Reply(HTTP_OK, "OK")
end
regGameEvent(GameEvent.HttpRead, onHttpRead)

local function luaSqlTestRead()
    for id, name, level in (Rows(objConn, "select id, name,level from player")) do
        Debug(string.format("id:%s, name %s, level %s", id, name, level))
    end
end

local function luaSqlWriteError()
    local tSql = {}
    table.insert(tSql, string.format("insert into test(name, sex) values ('name 1', 1)"))
    table.insert(tSql, string.format("insert into test(name2, sex) values ('name 2', 2)"))
    
    sqlExecAll(objConn, tSql)
end

local function luaSqlWriteOK()
    local tSql = {}
    table.insert(tSql, string.format("insert into test(name, sex) values ('name 1', 1)"))
    table.insert(tSql, string.format("insert into test(name, sex) values ('name 2', 2)"))
    
    sqlExecAll(objConn, tSql)
end

local function testRedis()
    Debug("test redis:")
    local client = redis.connect('192.168.73.128', 2222)
    --local client = redis.connect('127.0.0.1', 6379)
    local value = client:ping()
    Debug("ping retrun:" .. tostring(value))    
    client:set('usr:nrk', 10)
    client:set('usr:nobody', 5)
    value = client:get('usr:nrk')
    Debug("get usr:nrk " .. tostring(value))
    value = client:get('usr:nobody')
    Debug("get usr:nobody " .. tostring(value))
end

local function testpairsByKeys()
    local t = {}
    t[10] = 10
    t[50] = 30
    t[40] = 40
    t[70] = 10
    t[60] = 30
    t[80] = 40
    Debug("testpairsByKeys: use pairs:")
    for key, val in pairs(t) do
        print(key.."---"..val)
    end
    Debug("testpairsByKeys: use pairsByKeys:")
    for key, val in pairsByKeys(t) do
        print(key.."---"..val)
    end
end

local m_iMaxTime = 60 * 60 * 24 --* 30 * 12
local m_iMinRandTime = 1
local m_PreTime = 0
local m_iDelayEventRunCount = 0
local function OnDelayEvent(iTime)
    local uiCount = (g_objSessionMgr:getCount() * g_objSessionMgr:getTimer()) / 1000
    Debug(string.format("OnDelayEvent: need run time %d, now time %d", iTime, uiCount))
    if (iTime - m_PreTime) ~= 1 then
        Debug("OnDelayEvent: 1111 error.  error  error  error  error")
        fileWrite("test.txt", "%s\n", 
            string.format("pre time %d, now time %d", m_PreTime, iTime))
    end    
    m_PreTime = uiCount
    
    if (uiCount ~= iTime) then
        Debug("OnDelayEvent: 2222 error.  error  error  error  error")
        fileWrite("test.txt", "%s\n", 
            string.format("need run time %d, now time %d", iTime, uiCount))
    end
    
    if iTime == m_iMaxTime then
        fileWrite("test.txt", "%s\n", "OnDelayEvent run over")
    end
    
    m_iDelayEventRunCount = m_iDelayEventRunCount + 1
    Debug(string.format("OnDelayEvent: run count %d", m_iDelayEventRunCount))
end

local function TestDelayEvent()
    Debug("Begin register DelayEvent..")
    for i = 1, m_iMaxTime do
        regDelayEvent(i, OnDelayEvent, i)
    end
    Debug("End register DelayEvent..")
end

local m_iRandDelayEventCount = 30000
local function OnRandDelayEvent(iTime, uiCount, iID)
    local uiCurCount = (g_objSessionMgr:getCount() * g_objSessionMgr:getTimer()) / 1000
    Debug(string.format("OnRandDelayEvent: id %d, now time %d, need run time %d, reg time %d", 
        iID, uiCurCount, (iTime + uiCount), uiCount))
    if (uiCount + iTime) ~= uiCurCount then
        Debug("OnRandDelayEvent: error.  error  error  error  error")
        fileWrite("test2.txt", "%s\n", 
            string.format("id %d, now time %d, delay time %d, register time %d", 
            iID, uiCurCount, iTime, uiCount))
    end
    
    local iTime = math.random(m_iMinRandTime, m_iMaxTime)
    regDelayEvent(iTime, OnRandDelayEvent, iTime, uiCurCount, iID)
end

local function TestRandDelayEvent()
    Debug("Begin register RandDelayEvent..")
    local uiCount = (g_objSessionMgr:getCount() * g_objSessionMgr:getTimer()) / 1000
    for  i = 1, m_iRandDelayEventCount do
        local iTime = math.random(m_iMinRandTime, m_iMaxTime)
        regDelayEvent(iTime, OnRandDelayEvent, iTime, uiCount, i)
    end
    Debug("End register RandDelayEvent..")
end

local m_iRunDelayEventWhithRemoveCount = 0
local function onDelayEventWhithRemove(iID, uiToTick)
    local uiCurCount = (g_objSessionMgr:getCount() * g_objSessionMgr:getTimer()) / 1000
    Debug(string.format("onDelayEventWhithRemove: id %d now time %d, need run time %d ", 
        iID, uiCurCount, uiToTick))
    if uiCurCount ~= uiToTick then
        Debug("onDelayEventWhithRemove: error.  error  error  error  error")
        fileWrite("test3.txt", "%s\n", 
            string.format("id %d now time %d, delay time %d", 
            iID, uiCurCount, uiToTick))
    end
    
    m_iRunDelayEventWhithRemoveCount = m_iRunDelayEventWhithRemoveCount + 1
    Debug(string.format("onDelayEventWhithRemove: run count %d", m_iRunDelayEventWhithRemoveCount))
end

local function onRemoveDelayEvent(objTvecBase, iID, uiTimeOutTick)
    local iRegTick = objTvecBase:getRegTick()
    local iNowTick = getDelayEventTick()
    local iTimeOut = objTvecBase:getTime() - (iNowTick - iRegTick)
    
    removDelayEvent(objTvecBase)
    objTvecBase:setTime(iTimeOut)
    regDelayEventByBase(objTvecBase)
    
    if iTimeOut > 1 then
        local iRemoveTime = math.random(1, iTimeOut - 1)
        regDelayEvent(iRemoveTime, onRemoveDelayEvent, objTvecBase, iID, uiTimeOutTick)
    end
end

local m_iDelayEventWhithRemoveCount = 30000
local function TestDelayEventWhithRemove()
    Debug("Begin register DelayEventWhithRemove..")
    local uiCurTick = (g_objSessionMgr:getCount() * g_objSessionMgr:getTimer()) / 1000
    for i = 1, m_iDelayEventWhithRemoveCount do
        local iTime = math.random(m_iMinRandTime, m_iMaxTime)
        if 1 == iTime then
            iTime = iTime + 1
        end
        
        local iRemoveTime = math.random(1, iTime - 1)
        
        local objTvecBase = regDelayEvent(iTime, onDelayEventWhithRemove, i, uiCurTick + iTime)
        regDelayEvent(iRemoveTime, onRemoveDelayEvent, objTvecBase, i, uiCurTick + iTime)        
    end
    Debug("End register DelayEventWhithRemove..")
end

local function onDelayEventAtTime(strTime)
    local tNow = os.date("*t", time)
    Debug("req time:"..strTime)
    Debug(string.format("now time:%s:%s:%s",  tNow.hour, tNow.min, tNow.sec))
end

local function testDelayEventAtTime()
    local strTime1 = "11:20:59"
    local strTime2 = "10:01:25"
    
    regDelayEventAtTime(strTime1, onDelayEventAtTime, strTime1)
    regDelayEventAtTime(strTime2, onDelayEventAtTime, strTime2)
end

local function onStart()
    --luaSqlTestRead()
    --luaSqlWriteError()
    Debug("onStart")
    --TestRandDelayEvent()
    --TestDelayEvent()
    --TestDelayEventWhithRemove()
    testDelayEventAtTime()
    --testpairsByKeys()
    --testRedis()
end
regGameEvent(GameEvent.Start, onStart)

local function addLinker()
    g_objSessionMgr:addLinkOther("127.0.0.1", 15000, "test1")
    g_objSessionMgr:addLinkOther("127.0.0.1", 15000, "test2")
end

local function onStarted()
    g_Started = true
    --luaSqlWriteOK()
    Debug("onStarted")
    addLinker()
end
regGameEvent(GameEvent.Started, onStarted)
