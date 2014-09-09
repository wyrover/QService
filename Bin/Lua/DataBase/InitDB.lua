--[[
数据库连接初始化
--]]

--数据库类型
DBType_Game = 0
DBType_Log = DBType_Game + 1

--连接管理
if not LinkerMgr then
    LinkerMgr = LinkerManager:new()
end

--数据库管理
if not DBMgr then
    DBMgr = nil
end

--连接配置
local tDBConfig = 
{
    {Type = DBType_Game, Host = "127.0.0.1", Port = 3306, DB = "game", User = "root", PSW = "123456"},--游戏数据库
    {Type = DBType_Log, Host = "127.0.0.1", Port = 3306, DB = "log", User = "root", PSW = "123456"},--日志数据库
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

--玩家表字段
if not g_PlayerField then
    g_PlayerField = {}
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
    
    local objLinker = LinkerMgr:getLinker(DBType_Log)
    if not objLinker then
        return
    end
    
    local strSql = string.format("CREATE TABLE IF NOT EXISTS %s like %s", g_LogTableName, strLogTableTemplate)    
    objLinker:execDML(strSql)
end

--事件注册
local function OnSVStartUp()
    for _,val in ipairs(tDBConfig) do
        LinkerMgr:addLinker(val.Type, val.Host, val.DB, val.User, val.PSW, val.Port)
        Debug(string.format("open database %s.", val.DB))
    end
    
    --logs表处理   
    createLogTable()
    
    DBMgr = DBManager:new(LinkerMgr)
end
RegGameEvent(GameEvent_StartUp, "OnSVStartUp", OnSVStartUp)

local function OnSVShutDown()
    local objLinker = nil
    for _,val in ipairs(tDBConfig) do
        objLinker = LinkerMgr:getLinker(val.Type)
        if objLinker then
            objLinker:close()
            Debug(string.format("close database %s.", val.DB))
        end
    end
end
RegGameEvent(GameEvent_ShutDown, "OnSVShutDown", OnSVShutDown)

local function OnDayChange()
    createLogTable()
    DBMgr:getTableInfo()
end
RegGameEvent(GameEvent_DayChange, "OnDayChange", OnDayChange)

local function On1Hour()
    local objLinker = nil
    
    objLinker = LinkerMgr:getLinker(DBType_Game)
    if objLinker then
        objLinker:execDML(string.format("SELECT id FROM %s limit 1", g_PlayerTable))        
    end
    
    objLinker = LinkerMgr:getLinker(DBType_Log)
    if objLinker then
        objLinker:execDML(string.format("SELECT id FROM %s limit 1", strLogTableTemplate))
    end
end
RegGameEvent(GameEvent_1Hour, "On1Hour", On1Hour)
