--[[
数据库连接管理
--]]

DBType_Game = 0
DBType_Log = DBType_Game + 1

DBManager = {}
DBManager.__index = DBManager

--[[
描述：新建一数据库连接管理
参数：
返回值： 数据库连接管理
--]]
function DBManager:new()
        local self = {}        
        setmetatable(self, DBManager)
        self.Linkers = {}
        
        return self
end

--[[
描述：添加一连接
参数：
返回值： 无
--]]
function DBManager:addLinker(DBType, strHost, strDBName, strUser, strPSW, usPort)
    local dbUrl = CDBUrl()
    dbUrl:setUrl(string.format("DB=%s;UserName=%s;PassWord=%s;Port=%d;Host=%s", 
    strDBName, strUser, strPSW, usPort, strHost))
    
    local Linker = CMySQLLink()
    Linker:open(dbUrl)
        
    self.Linkers[DBType] = Linker
end

--[[
描述：获取一连接
参数：
返回值： 连接
--]]
function DBManager:getLinker(DBType)
    return self.Linkers[DBType]
end
