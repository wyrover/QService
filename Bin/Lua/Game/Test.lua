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
    local client = redis.connect('127.0.0.1', 6379)
    local value = client:ping()
    Debug("ping retrun:" .. tostring(value))    
    client:set('usr:nrk', 10)
    client:set('usr:nobody', 5)
    value = nil
    value = client:get('usr:nrk')
    Debug("get usr:nrk " .. tostring(value))
    value = nil
    value = client:get('usr:nobody')
    Debug("get usr:nobody " .. tostring(value))
end

local function onStart()
    --luaSqlTestRead()
    --luaSqlWriteError()
    Debug("onStart") 
    testRedis()
end
regGameEvent(GameEvent.Start, onStart)

local function onStarted()
    g_StartCompleted = true
    --luaSqlWriteOK()
    Debug("onStarted")
end
regGameEvent(GameEvent.Started, onStarted)

local iCount = 0
local function testDBLog()
    iCount = iCount + 1
    local strMsg = string.format("%d", iCount)
    Q_DBLOG("13245346543", -1, strMsg, string.len(strMsg)) 
end

local function testTxtLog()
    local str = "!"
    for i = 1, 12 do
        str = str .. "a"
    end
    str = str .. "!"    
    Q_LOG(LOGLV_INFO, str)
    
    str = "!"
    for i = 1, 655 do
        str = str .. "a"
    end
    str = str .. "!"
    Q_LOG(LOGLV_INFO, str)
    
    str = "!"
    for i = 1, 65535 do
        str = str .. "a"
    end
    str = str .. "!"
    Q_LOG(LOGLV_INFO, str)
end

local function onFiveSecond()
    --testDBLog()
    --testTxtLog()
end
regGameEvent(GameEvent.FiveSecond, onFiveSecond)
