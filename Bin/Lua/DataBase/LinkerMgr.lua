--[[
数据库连接管理
--]]

LinkerManager = {}
LinkerManager.__index = LinkerManager

--[[
描述：新建一数据库连接管理
参数：
返回值： 数据库连接管理
--]]
function LinkerManager:new()
        local self = {}        
        setmetatable(self, LinkerManager)
        self.Linkers = {}
        self.DBName = {}
        
        return self
end

--[[
描述：添加一连接
参数：
返回值： 无
--]]
function LinkerManager:addLinker(DBType, strHost, strDBName, strUser, strPSW, usPort)
    local dbUrl = CDBUrl()
    dbUrl:setUrl(string.format("DB=%s;UserName=%s;PassWord=%s;Port=%d;Host=%s", 
    strDBName, strUser, strPSW, usPort, strHost))
    
    local Linker = CMySQLLink()
    Linker:open(dbUrl)
    
    self.DBName[DBType] = strDBName
    self.Linkers[DBType] = Linker
end

--[[
描述：获取一连接
参数：
返回值： 连接
--]]
function LinkerManager:getLinker(DBType)
    return self.Linkers[DBType]
end

--[[
描述：获取数据库名
参数：
返回值： 连接
--]]
function LinkerManager:getDBName(DBType)
    return self.DBName[DBType]
end

--[[
描述：获取所有连接
参数：
返回值： table
--]]
function LinkerManager:getAllLinker()
    return self.Linkers
end
