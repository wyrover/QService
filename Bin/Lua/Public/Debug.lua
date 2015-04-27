--[[
Ö´ÐÐÃüÁî
--]]

--[[
ÃèÊö£ºÖ´ÐÐ×Ö·û´®ÃüÁî
²ÎÊý£º×Ö·û´®
·µ»ØÖµ£ºÎÞ
--]]
local function doString(strMsg)
    local Func = load(strMsg) 
    
    return callFunc(Func)
end


local function getIndent(level)
    return string.rep("    ", level)
end

local function dump(obj, offset)
    local dumpObj
    offset = offset or 0

    local function quoteStr(str)
        str = string.gsub(str, "[%c\\\"]", {
            ["\t"] = "\\t",
            ["\r"] = "\\r",
            ["\n"] = "\\n",
            ["\""] = "\\\"",
            ["\\"] = "\\\\",
        })
        return '"' .. str .. '"'
    end
    local function wrapKey(val)
        if type(val) == "number" then
            return "[" .. val .. "]"
        elseif type(val) == "string" then
            return "[" .. quoteStr(val) .. "]"
        else
            return "[" .. tostring(val) .. "]"
        end
    end
    local function wrapVal(val, level)
        if type(val) == "table" then
            return dumpObj(val, level)
        elseif type(val) == "number" then
            return val
        elseif type(val) == "string" then
            return quoteStr(val)
        else
            return tostring(val)
        end
    end
    local isArray = function(arr)
        local count = 0 
        for k, v in pairs(arr) do
            count = count + 1 
        end 
        for i = 1, count do
            if arr[i] == nil then
                return false
            end 
        end 
        return true, count
    end

    dumpObj = function(obj, level)
        if type(obj) ~= "table" then
            return wrapVal(obj)
        end
        --level = level + 1
        local tokens = {}
        tokens[#tokens + 1] = "\n"..getIndent(level).."{"
        --tokens[#tokens + 1] = "{"
        local ret, count = isArray(obj)
        if ret then
            for i = 1, count do
                tokens[#tokens + 1] = getIndent(level + 1) .. wrapVal(obj[i], level + 1) .. ","
            end
        else
            for k, v in pairs(obj) do
                tokens[#tokens + 1] = getIndent(level + 1) .. wrapKey(k) .. " = " .. wrapVal(v, level + 1) .. ","
            end
        end
        tokens[#tokens + 1] = getIndent(level) .. "}"
        return table.concat(tokens, "\n")
    end
    return dumpObj(obj, offset)
end

function debug.var_export(obj, prt)
    if prt then
        print(dump(obj))
    else
        return dump(obj)
    end
end

function debug.var_dump(obj)
    print(dump(obj))
end

function debug.trace(depth, asStr, baseDepth)
    local sInfo = "stack traceback:\n"

    -- escape trace function stacks
    if not baseDepth then
        baseDepth = 3
    else
        baseDepth = baseDepth + 3
    end

    local iIndent = 0
    
    local function doTrace(curLevel, baseLevel)
        local stack = debug.getinfo(curLevel)
        if not stack then 
            return false 
        end

        local label = curLevel - baseLevel + 1

        if stack.what == "C" then
            sInfo = sInfo..string.format("%d.[C FUNCTION]\n",label)
        else
            sInfo = sInfo..string.format("[%s]:%s:%s:%s:\n",
                label, stack.short_src, stack.currentline, 
                stack.name or "")
        end

        local i = 1
        while true do
            local paramName, paramVal = debug.getlocal(curLevel, i)
            if not paramName then 
                break 
            end
            sInfo = sInfo..getIndent(iIndent + 1)..string.format("%s = %s\n", 
                paramName, dump(paramVal, iIndent + 2))
            i = i + 1
        end

        return true
    end

    local i = baseDepth
    repeat
        local ret = doTrace(i, baseDepth)
        i = i + 1
        if depth and i >= depth + baseDepth then
            break
        end
    until ret ~= true

    sInfo = sInfo.."\n\n"

    if asStr then
        return sInfo
    else
        print(sInfo)
    end 
end
