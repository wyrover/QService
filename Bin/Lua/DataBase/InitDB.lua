--[[
数据库连接初始化
--]]

--连接管理
if not DBMgr then
    DBMgr = DBManager:new()
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
--当前log表名
if not g_LogTableName then
    g_LogTableName = nil
end

--[[
描述：创建日志表
参数：
返回值： 无
--]]
local function CreateLogTable()
    local tNow = os.date("*t", time)
    g_LogTableName = string.format("logs_%d_%d", tNow.year, tNow.month)
    Debug("log table name:"..g_LogTableName)
    
    local objLinker = DBMgr:getLinker(DBType_Log)
    if not objLinker then
        return
    end
    
    local strSql = string.format("CREATE TABLE IF NOT EXISTS %s like %s", g_LogTableName, strLogTableTemplate)    
    objLinker:execDML(strSql)
end

--事件注册
local function OnSVStartUp()
    for _,val in ipairs(tDBConfig) do
		DBMgr:addLinker(val.Type, val.Host, val.DB, val.User, val.PSW, val.Port)
		Debug(string.format("open database %s.", val.DB))
	end
    
    --logs表处理   
    CreateLogTable()
end
RegGameEvent(GameEvent_StartUp, "OnSVStartUp", OnSVStartUp)

local function OnSVShutDown()
	local objLinker = nil
    for _,val in ipairs(tDBConfig) do
		objLinker = DBMgr:getLinker(val.Type)
		if objLinker then
			objLinker:close()
			Debug(string.format("close database %s.", val.DB))
		end
	end
end
RegGameEvent(GameEvent_ShutDown, "OnSVShutDown", OnSVShutDown)

local function OnDayChange()
    CreateLogTable()
end
RegGameEvent(GameEvent_DayChange, "OnDayChange", OnDayChange)

local function On1Hour()
	local objLinker = nil
	
	objLinker = DBMgr:getLinker(DBType_Game)
	if objLinker then
		objLinker:execDML("SELECT id FROM player limit 1")		
	end
	
	objLinker = DBMgr:getLinker(DBType_Log)
	if objLinker then
		objLinker:execDML("SELECT id FROM logs limit 1")
	end
end
RegGameEvent(GameEvent_1Hour, "On1Hour", On1Hour)
