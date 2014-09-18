--[[
数据库操作
--]]

--数据库类型
DBType_Game = 0
DBType_Log = DBType_Game + 1

--mysql初始化
if not g_MysqlENV then
    g_MysqlENV = assert(mysql.mysql())
end

DBManager = {}
DBManager.__index = DBManager

--[[
描述：数据库操作
参数：
返回值： table
--]]
function DBManager:new()
    local self = {}
    setmetatable(self, DBManager)
        
    self.Linker = {}
        
    return self
end

--[[
描述: 添加链接
参数: 
返回值: 
--]]
function DBManager:addLinker(DBType, strDBName, strUser, strPSW, strHost, usPort)
    self.Linker[DBType] = assert(g_MysqlENV:connect(strDBName, strUser, strPSW, strHost,usPort))
end

--[[
描述: 获取链接
参数: 
返回值: 
--]]
function DBManager:getLinker(DBType)
    return self.Linker[DBType]
end

--[[
描述: 关闭连接，清理
参数: 
返回值: 
--]]
function DBManager:closeAll()
    for _, Linker in pairs(self.Linker) do
        Linker:close()
    end
    
    if g_MysqlENV then
        g_MysqlENV:close()
    end
end

--[[
描述: 执行sql
参数: 
返回值: 执行结果
--]]
function DBManager:executeSql(DBType, strSql)
    local objLinker = self:getLinker(DBType)
    assert(objLinker, string.format("get linker by type %d error.", DBType))
    
    return assert(objLinker:execute(strSql))
end

--[[
描述: 批量执行sql
参数: 
返回值: 
--]]
function DBManager:executeAll(DBType, tSqls)
    local objLinker = self:getLinker(DBType)
    if not objLinker then
        Debug(string.format("get linker by type %d error.", DBType))
        return
    end
    
    if IsTableEmpty(tSqls) then
        return
    end
    
    local bRtn = objLinker:setautocommit(false)
    if bRtn then
        for _, sql in pairs(tSqls) do
            local Rtn, strError = objLinker:execute(sql)
            if not Rtn then
                objLinker:rollback()
                objLinker:setautocommit(true)
                assert(Rtn, strError)
                
                return
            end
        end
            
        objLinker:commit()
        objLinker:setautocommit(true)
        
        return
    end
  
    Debug("setautocommit error.")
    for _, sql in pairs(tSqls) do
        assert(objLinker:execute(sql))
    end
end

--[[
描述: 插入
参数: tAttrs -- {属性、字段 = 值}
返回值: 
--]]
function DBManager:insert(DBType, strTable, tAttrs)
    if IsTableEmpty(tAttrs) then
        return
    end
    
    local tSqls = {}
    
    --构造语句
    for _, tAttr in pairs(tAttrs) do
        local strSql = string.format("INSERT INTO %s (", strTable)
        for strField, _ in pairs(tAttr) do
            strSql = string.format("%s %s,", strSql, strField)
        end
        
        strSql = string.sub(strSql, 1, string.len(strSql) - 1)
        strSql = string.format("%s ) VALUES (", strSql)
        
        for strFiled, Attr in pairs(tAttr) do
            local strType = type(Attr)
            if "number" ==  strType then
                strSql = string.format("%s %d,", strSql, Attr)
            elseif "boolean" == strType then
                if Attr then
                    strSql = string.format("%s %d,", strSql, 1)
                else
                    strSql = string.format("%s %d,", strSql, 0)
                end
            elseif "string" == strType then
                strSql = string.format("%s '%s',", strSql, Attr)
            elseif "table" == strType then
                strSql = string.format("%s '%s',", strSql, cjson.encode(AttrInfo))
            else
                Debug("unknown field type")
                return
            end
        end
        
        strSql = string.sub(strSql, 1, string.len(strSql) - 1)
        strSql = string.format("%s )", strSql)        
        table.insert(tSqls, strSql)
    end
    
    self:executeAll(DBType, tSqls)
end

--[[
描述: 更新,必须包含id字段
参数: tAttrs -- id = {属性、字段 = 值}
返回值: 
--]]
function DBManager:update(DBType, strTable, tAttrs)
    if IsTableEmpty(tAttrs) then
        return
    end
    
    local tSqls = {}
    
    for id, tAttr in pairs(tAttrs) do
        local strSql = string.format("UPDATE %s SET", strTable)
        for strFiled, Attr in pairs(tAttr) do
            local strType = type(Attr)
            if ("number" == strType) then
                strSql = string.format("%s %s=%d,", strSql, strFiled, Attr)
            elseif("boolean" == strType) then
                if AttrInfo then
                    strSql = string.format("%s %s=%d,", strSql, strFiled, 1)
                else
                    strSql = string.format("%s %s=%d,", strSql, strFiled, 0)
                end                
            elseif("string" == strType) then 
                strSql = string.format("%s %s='%s',", strSql, strFiled, Attr)
            elseif("table" == strType) then
                strSql = string.format("%s %s='%s',", strSql, strFiled, cjson.encode(Attr))
            else
                Debug("unknown field type")
                return
            end        
        end
        
        --去掉最后个,号
        strSql = string.sub(strSql, 1, string.len(strSql) - 1)
        strSql = string.format("%s WHERE id=%s", strSql, id)
        table.insert(tSqls, strSql)
    end
    
    self:executeAll(DBType, tSqls)
end
