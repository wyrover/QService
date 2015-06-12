--[[
一些常用函数
--]]

--Debug打印开关 影响Debug table.print
local bDebug = true

--[[
描述：日志
参数：
返回值：无
--]]
function Log(iLevel, strFormat, ...)
    local strMsg = string.format(strFormat, table.unpack({...}))
    if bDebug then
        print(strMsg)
    end
    
    Q_LOG(iLevel, strMsg)
end

--[[
描述：调试信息打印
参数：
返回值：无
--]]
function Debug(strFormat, ...)
    if bDebug then
        local strMsg = string.format(strFormat, table.unpack({...}))
        print(string.format("[%s][Lua_Debug] %s", os.date(), strMsg))
    end
end

--[[
描述：写文件
参数：
返回值：无
--]]
function fileWrite(strFile, strFormat, ...)
    local pFile = io.open(strFile, "a")
    if not pFile then
        return
    end
    
    pFile:write(string.format(strFormat, table.unpack({...})))
    pFile:close()
end

--[[
描述：获取文件名
参数：
返回值：
--]]
function getFileName(str)
    local idx = str:match(".+()%.%w+$")
    if(idx) then
        return str:sub(1, idx-1)
    else
        return str
    end
end

--[[
描述：获取文件扩展名
参数：
返回值：
--]]
function getExtension(str)
    return str:match(".+%.(%w+)$")
end

--[[
描述：获取文件夹下所有文件名
参数：strPath --路径 strExtension --扩展名 
返回值：table
--]]
function getAllFile(strPath, strExtension, tAllFile)
    if not tAllFile then
        tAllFile = {}
    end
    
    for file in lfs.dir(strPath) do
        if file ~= "." and file ~= ".." then
            local strFile = string.format("%s%s%s", strPath, Q_GetPathSeparator(), file)
            local tAttr = lfs.attributes(strFile)
            if "directory" == tAttr.mode then
                getAllFile(strFile, strExtension, tAllFile)
            else
                if (not strExtension) 
                    or ("*" == strExtension) 
                    or (0 == string.len(strExtension)) then
                    table.insert(tAllFile, file)
                else
                    if string.upper(strExtension) == string.upper(getExtension(file)) then
                        table.insert(tAllFile, file)
                    end
                end
            end
        end
    end
    
    return tAllFile
end

--[[
描述：加载lua文件
参数：
返回值： 
--]]
function loadFile(tFile)
    local luaDir = Q_GetModulPath() .. "Lua" .. Q_GetPathSeparator()
    local strPath = ""
    
    for _, val in pairs(tFile) do
        if val[2] then
            strPath = luaDir .. val[1] .. ".lua"
            dofile(strPath)
        end
    end
end

--[[
描述：保护模式中调用函数
参数：
返回值：bool 函数返回值
--]]
function callFunc(Func, ...)
    assert("function" == type(Func))

    local function onExcept(strMsg)
        local strStack = debug.trace(3, true, 2)
        Debug("error message: %s", strMsg)
        Debug("trace: %s", strStack)
        Log(LOGLV_ERROR, "error message: %s", strMsg)
        Log(LOGLV_ERROR, "trace: %s", strStack)
    end
    
    return xpcall(Func, onExcept, table.unpack({...}))
end

--[[
描述：数字为key的table 按key顺序遍历
参数：
返回值：
--]]
function pairsByKeys(t)  
    local a = {}
    for n in pairs(t) do  
        a[#a+1] = n  
    end
    table.sort(a)  
    local i = 0
    return function()
        i = i + 1
        return a[i], t[a[i]]  
    end  
end

--[[
描述：lua sql
参数：
返回值：读取到的值
--]]
function Rows(objConn, strSql)
    local cursor = assert (objConn:execute (strSql))
    return function()
        return cursor:fetch()
    end
end

--[[
描述: 批量执行sql
参数: 
返回值: 
--]]
function sqlExecAll(objConn, tSqls)
    if table.empty(tSqls) then
        return
    end
    
    local bRtn = objConn:setautocommit(false)
    if bRtn then
        for _, sql in pairs(tSqls) do
            local Rtn, strError = objConn:execute(sql)
            if not Rtn then
                objConn:rollback()
                objConn:setautocommit(true)
                assert(Rtn, strError)
                
                return
            end
        end
            
        objConn:commit()
        objConn:setautocommit(true)
        
        return
    end
  
    for _, sql in pairs(tSqls) do
        assert(objConn:execute(sql))
    end
end

--[[
描述：字符串拆分
参数：
返回值：table
--]]
function string.split(str, delimiter)
    assert("string" == str)
    assert("string" == delimiter)
    
    if ('' == delimiter) then 
        return {str} 
    end
    
    local pos,arr = 0, {}
    for st,sp in function() return string.find(str, delimiter, pos, true) end do
        table.insert(arr, string.sub(str, pos, st - 1))
        pos = sp + 1
    end
    
    table.insert(arr, string.sub(str, pos))
    
    return arr
end

--[[
描述：table打印
参数：
返回值：无
--]]
function table.print(lua_table, indent)
    if not bDebug then
        return
    end
    
    assert("table" == type(lua_table))    
    indent = indent or 0
    for k, v in pairs(lua_table) do
        if type(k) == "string" then
            k = string.format("%q", k)
        end
        
        local szSuffix = ""
        if type(v) == "table" then
            szSuffix = "{"
        end
        
        local szPrefix = string.rep("    ", indent)
        formatting = szPrefix.."["..k.."]".." = "..szSuffix
        
        if type(v) == "table" then
            print(formatting)
            table.print(v, indent + 1)
            print(szPrefix.."},")
        else
            local szValue = ""
            if type(v) == "string" then
                szValue = string.format("%q", v)
            else
                szValue = tostring(v)
            end
            
            print(formatting..szValue..",")
        end
    end
end

--[[
描述：table是否为空
参数：
返回值：bool
--]]
function table.empty(lua_table)
    assert("table" == type(lua_table))    
    for _, _ in pairs(lua_table) do
        return false
    end
    
    return true
end

--[[
描述：table长度
参数：
返回值：int
--]]
function table.len(lua_table)
    assert("table" == type(lua_table))    
    local iCount = 0
    for _, _ in pairs(lua_table) do
        iCount = iCount + 1
    end
    
    return iCount
end

--[[
描述：table拷贝,不考虑带环的
参数：
返回值：table
--]]
function table.copy(tTable)
    assert("table" == type(lua_table))    
    local tNewTab = {}  
    for i, v in pairs(tTable) do  
        local vtyp = type(v)
        
        if ("table" == vtyp) then  
            tNewTab[i] = table.copy(v)  
        elseif ("thread" == vtyp) then  
            tNewTab[i] = v  
        elseif ("userdata" == vtyp) then  
            tNewTab[i] = v  
        else  
            tNewTab[i] = v  
        end
    end  
    
    return tNewTab
end

--[[
描述：返回类似enum的table
参数：
返回值：table
--]]
function table.enum(tMsg, iBegin) 
    assert("table" == type(tMsg))    
    local tEnum = {} 
    local iEnumIndex = iBegin or 0 
    for key, val in pairs(tMsg) do 
        tEnum[val] = iEnumIndex + key - 1
    end 
    
    return tEnum 
end 
