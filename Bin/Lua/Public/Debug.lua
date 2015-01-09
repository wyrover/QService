--[[
执行命令
--]]

--[[
描述：执行字符串命令
参数：字符串
返回值：无
--]]
local function doString(strMsg)
    local Func = load(strMsg) 
    
    return callFunc(Func)
end
