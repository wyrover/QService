--[[
数据库连接初始化
--]]

--数据库管理
if not DBMgr then
    DBMgr = DBManager:new()
end

--连接配置
local tDBConfig = 
{
    {Type = DBType.Game, Host = "127.0.0.1", Port = 3306, DB = "game", User = "root", PSW = "123456"},--游戏数据库
    {Type = DBType.Log, Host = "127.0.0.1", Port = 3306, DB = "log", User = "root", PSW = "123456"},--日志数据库
}

--服务器注册检查
g_CheckSVRegFun = checkSVReg

--log表模板
local strLogTableTemplate = "logs"
--玩家表名
g_PlayerTable = "player"

--当前log表名
if not g_LogTableName then
    g_LogTableName = nil
end

--[[
描述：创建日志表
参数：
返回值： 无
--]]
local function createLogTable()
    local tNow = os.date("*t", time)
    g_LogTableName = string.format("logs_%d_%d", tNow.year, tNow.month)
    Debug("log table name:"..g_LogTableName)
    
    local strSql = string.format("CREATE TABLE IF NOT EXISTS %s like %s", g_LogTableName, strLogTableTemplate)    
    DBMgr:executeSql(DBType.Log, strSql)
end

--事件注册
local function OnSVStartUp()
    for _,val in ipairs(tDBConfig) do
        DBMgr:addLinker(val.Type, val.DB, val.User, val.PSW, val.Host, val.Port)
        Debug(string.format("open database %s.", val.DB))
    end
    
    --logs表处理   
    createLogTable()
end
RegGameEvent(GameEvent.StartUp, "OnSVStartUp", OnSVStartUp)

local function OnSVShutDown()
    DBMgr:closeAll()
end
RegGameEvent(GameEvent.ShutDown, "OnSVShutDown", OnSVShutDown)

local function OnDayChange()
    createLogTable()
end
RegGameEvent(GameEvent.DayChange, "OnDayChange", OnDayChange)

local function On1Hour()    
    DBMgr:executeSql(DBType.Game, string.format("SELECT id FROM %s limit 1", g_PlayerTable))
    DBMgr:executeSql(DBType.Log, string.format("SELECT id FROM %s limit 1", strLogTableTemplate))
end
RegGameEvent(GameEvent.OneHour, "On1Hour", On1Hour)
