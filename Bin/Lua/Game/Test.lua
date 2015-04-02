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

local function onStart()
    --luaSqlTestRead()
    --luaSqlWriteError()
    Debug("onStart")    
end
regGameEvent(GameEvent.Start, onStart)

local function onStarted()
    g_StartCompleted = true
    --luaSqlWriteOK()
    Debug("onStarted")
end
regGameEvent(GameEvent.Started, onStarted)
