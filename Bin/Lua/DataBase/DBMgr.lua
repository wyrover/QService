--[[
数据库操作
--]]

DBManager = {}
DBManager.__index = DBManager

--[[
描述：数据库操作
参数：
返回值： table
--]]
function DBManager:new(LinkerManager)
        local self = {}
        setmetatable(self, DBManager)
        
        self.LinkerMgr = LinkerManager
        self.Filed = {}
        self:getTableInfo()
        
        return self
end

--[[
描述：从数据库获取所有表结构信息
参数：
返回值： 无
--]]
function DBManager:getTableInfo()
    self.Filed = {}
    
    local LinkerManager = self.LinkerMgr
    for key, val in pairs(LinkerManager:getAllLinker()) do
        local strDBName = LinkerManager:getDBName(key)
        local tTableInfo = {}
            
        --table
        local objQueryTable = val:execQuery("show tables")            
        while not objQueryTable:eof() do
            local strTableName = objQueryTable:getStringField(string.format("Tables_in_%s", strDBName), "")
                
            --field
            tTableInfo[strTableName] = self:getTableFiled(val, strTableName)
                
            objQueryTable:nextRow()
        end
            
        self.Filed[key] = tTableInfo
        objQueryTable:finalize()
    end
        
    Debug("tables information:")
    PrintTable(self.Filed)
end

--[[
描述：从数据库获取表结构信息
参数：
返回值： 无
--]]
function DBManager:getTableFiled(objLinker, strTable)
    local strShowField = string.format("show columns from %s", strTable)                 
    local tFieldInfo = {}
    
    local objQueryField = objLinker:execQuery(strShowField)    
                
    --字段
    while not objQueryField:eof() do
        local strField = objQueryField:getStringField("Field", "")
        local strType = objQueryField:getStringField("Type", "")
        local iBegin = string.find(strType, "%(")
        if iBegin then
            strType = string.sub(strType, 1, iBegin - 1)
        end

        tFieldInfo[strField] = strType
   
        objQueryField:nextRow()
    end
    
    objQueryField:finalize()
    
    return tFieldInfo
end

--[[
描述：获取连接
参数：
返回值： 无
--]]
function DBManager:getLinker(DBType)
    return self.LinkerMgr:getLinker(DBType)
end

--[[
描述：增加字段
参数：
返回值： 无
--]]
function DBManager:addField(DBType, strTable, strField, strDBType)
    if not self.Filed[DBType] then
        return
    end
    
    if not self.Filed[DBType][strTable] then
        return
    end
    
    self.Filed[DBType][strTable][strField] = strDBType
end

--[[
描述：获取字段类型
参数：
返回值: string or nil
--]]
function DBManager:getFieldType(DBType, strTable, strField)
    if not self.Filed[DBType] then
        return nil
    end
    
    if not self.Filed[DBType][strTable] then
        return nil
    end
    
    return self.Filed[DBType][strTable][strField]
end

--[[
描述：获取表字段信息
参数：
返回值： 无
--]]
function DBManager:getTableFieldInfo(DBType, strTable)
    if not self.Filed[DBType] then
        return nil
    end
    
    return self.Filed[DBType][strTable]
end

--[[
描述：根据属性创建字段
参数：
返回值： 无
--]]
function DBManager:createField(objLinker, DBType, strTable, tAttrs)
    local tSqls = {}
    local tNewFiled = {}
    
    for _, tAttr in pairs(tAttrs) do
        for strField, Attr in pairs(tAttr) do
            local strDBType = nil
            local strAddFiledSql = nil
            local strAttrType = type(Attr)
            
            local strDBType = self:getFieldType(DBType, strTable, strField)            
            if (not strDBType) and (not tNewFiled[strField]) then
                if ("number" == strAttrType) or ("boolean" == strAttrType) then
                    strAddFiledSql = string.format("ALTER TABLE %s ADD COLUMN `%s` INTEGER NOT NULL DEFAULT 0",
                        strTable, strField)
                    strDBType = "int"
                elseif("string" == strAttrType) or ("table" == strAttrType) then
                    strAddFiledSql = string.format("ALTER TABLE %s ADD COLUMN `%s` TEXT",
                        strTable, strField)
                    strDBType = "text"
                else
                    Debug("unknown field type")                
                    return false
                end
                
                table.insert(tSqls, strAddFiledSql) 
                tNewFiled[strField] = strDBType
                Debug(strAddFiledSql)
            end
        end
    end
    
    if not IsTableEmpty(tSqls) then
        objLinker:Transaction()
        for _, sql in pairs(tSqls) do
            objLinker:execDML(sql)
        end
        objLinker:commitTransaction()
    end
    
    for key, val in pairs(tNewFiled) do
        self:addField(DBType, strTable, key, val)
    end
    
    return true
end

--[[
描述：插入属性
参数：
返回值： 无
--]]
function DBManager:insert(DBType, strTable, tAttrs)
    if IsTableEmpty(tAttrs) then
        return
    end
    
    local objLinker = self:getLinker(DBType)
    if not objLinker then
        return
    end
       
    if not self:createField(objLinker, DBType, strTable, tAttrs) then
        return
    end
    
    local tSqls = {}
    
    for _, tAttr in pairs(tAttrs) do
        local strSql = string.format("INSERT INTO %s (", strTable)
        for strField, _ in pairs(tAttr) do
            strSql = string.format("%s %s,", strSql, strField)
        end
        
        strSql = string.sub(strSql, 1, string.len(strSql) - 1)
        strSql = string.format("%s ) VALUES (", strSql)
        
        for strFiled, Attr in pairs(tAttr) do
            local strType = type(Attr)
            local tDBType = self:getFieldType(DBType, strTable, strFiled)
            if "bigint" == tDBType then
                strSql = string.format("%s %s,", strSql, Attr)
            else
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
        end
        
        strSql = string.sub(strSql, 1, string.len(strSql) - 1)
        strSql = string.format("%s )", strSql)
        Debug(strSql)
        table.insert(tSqls, strSql)
    end
    
    if not IsTableEmpty(tSqls) then
        objLinker:Transaction()
        for _, sql in pairs(tSqls) do
            objLinker:execDML(sql)
        end
        objLinker:commitTransaction()
    end
end

--[[
描述：更新属性
参数：
返回值： 无
--]]
function DBManager:update(DBType, strTable, tAttrs)
    if IsTableEmpty(tAttrs) then
        return
    end
    
    local objLinker = self:getLinker(DBType)
    if not objLinker then
        return
    end
     
    if not self:createField(objLinker, DBType, strTable, tAttrs) then
        return
    end
    
    local tSqls = {}
    
    for id, tAttr in pairs(tAttrs) do
        local strSql = string.format("UPDATE %s SET", strTable)
        for strFiled, Attr in pairs(tAttr) do
            local strType = type(Attr)
            local tDBType = self:getFieldType(DBType, strTable, strFiled)
            if "bigint" == tDBType then
                strSql = string.format("%s %s=%s,", strSql, strFiled, Attr)
            else
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
        end
        
        --去掉最后个,号
        strSql = string.sub(strSql, 1, string.len(strSql) - 1)
        strSql = string.format("%s WHERE id=%s", strSql, id)
        Debug(strSql)
        table.insert(tSqls, strSql)
    end
    
    if not IsTableEmpty(tSqls) then
        objLinker:Transaction()
        for _, sql in pairs(tSqls) do
            objLinker:execDML(sql)
        end
        objLinker:commitTransaction()
    end
end

--[[
描述：加载属性信息
参数：
返回值： 无
--]]
function DBManager:loadInfo(DBType, strTable, strID)
    local tTableInfo = self:getTableFieldInfo(DBType, strTable)
    if not tTableInfo then
        return nil
    end
    
    local objLinker = self:getLinker(DBType)
    if not objLinker then
        return nil
    end
    
    local strSql = nil    
    if not strID then
        strSql = string.format("SELECT * FROM %s", strTable)
    else
        strSql = string.format("SELECT * FROM %s WHERE id = %s", strTable, strID)
    end
    
    local tAllInfo = {}
    local objQuery = objLinker:execQuery(strSql)
    while not objQuery:eof() do
        local  tInfo = {}
        
        for strField, strType in pairs(tTableInfo) do
            if "bigint" == strType then
                tInfo[strField] = objQuery:getStrInt64Field(strField, 0)
            elseif "int" == strType then
                tInfo[strField] = objQuery:getIntField(strField, 0)
            else
                tInfo[strField] = objQuery:getStringField(strField, "")
            end
        end
        
        table.insert(tAllInfo, tInfo)
        
        objQuery:nextRow()
    end
    
    objQuery:finalize()
    
    if IsTableEmpty(tAllInfo) then
        return nil
    end
    
    return tAllInfo
end
