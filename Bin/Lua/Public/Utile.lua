--[[
一些常用函数
--]]

--Debug打印开关
local bDebugSwitch = true

--计时器
local objCtimer = CTimer()

--唯一ID
local objSnowflakeID = CSnowflakeID()
objSnowflakeID:setMachineID(getServerID())

--过滤敏感字
if not objFilter then
    objFilter = CFilter()
end

--字符集
local objCharset = CCharset()

--[[
描述：调试信息打印
参数：
返回值：无
--]]
function Debug(msg)
    if bDebugSwitch then
        print(string.format("[%s][Lua_Debug] %s", os.date(), tostring(msg)))
    end
end

--[[
描述：获取字符串编码格式
参数：
返回值：编码格式
--]]
function getCharset(strWord)
    return objCharset:getStrCharset(strWord, string.len(strWord))
end

--[[
描述：获取一不重复的ID
参数：
返回值：ID
--]]
function getID()
    return objSnowflakeID:getSnowflakeID()
end

--[[
描述：关闭
参数：
返回值： 无
--]]
function closeLink(iClentID)
    g_objSessionMgr:closeLinkByID(iClentID)
end

--[[
描述：添加敏感词
参数：
返回值：无
--]]
function addFilterWord(strWord)
    objFilter:addSensitiveWord(strWord, string.len(strWord))
end

--[[
描述：是否包含敏感词
参数：
返回值：bool
--]]
function checkFilterWord(strWord)
    return objFilter:checkHave(strWord, string.len(strWord))
end

--[[
描述：过滤敏感词
参数：
返回值：过滤后的结果
--]]
function Filter(strWord)
    return objFilter:Filter(strWord, string.len(strWord))
end

--[[
描述：重置计时器
参数：
返回值：无
--]]
function timerReStart()
    objCtimer:reStart()
end

--[[
描述：获取耗时
参数：
返回值：double
--]]
function timerElapsed()
    return objCtimer:Elapsed()
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
描述：保护模式中调用函数
参数：
返回值：bool 函数返回值
--]]
function callFunc(Func, ...)
    assert("function" == type(Func))

    local function onExcept(strMsg)
        local strStack = debug.trace(3, true, 2)
        Debug("error message:" .. strMsg)
        Debug("trace:" .. strStack)
        Q_LOG(LOGLV_ERROR, "error message:" .. strMsg)
        Q_LOG(LOGLV_ERROR, "trace:" .. strStack)
    end
    
    return xpcall(Func, onExcept, table.unpack({...}))
end

--[[
描述：lua sql
参数：
返回值：读取到的值
--]]
function Rows(objConn, strSql)
    local cursor = assert (objConn:execute (strSql))
    return function ()
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
