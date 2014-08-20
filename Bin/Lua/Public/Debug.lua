--[[
执行命令
--]]

--[[
描述：执行字符串命令
参数：
返回值：无
--]]
local function lua_DoStr(tbMessage)
    local Func = load(tbMessage[Protocol_DebugStr])
    local tRtn = {}
    tRtn[Protocol_Request] = Debug_DoStringRtn
    if Func then
        local bRtn, rtnMsg = CallFunc(Func)
        if bRtn then
            tRtn[Protocol_RTN] = Q_RTN_OK
        else
            tRtn[Protocol_RTN] = Q_RTN_FAILE
        end
        
        tRtn[Protocol_DebugStrRtn] = rtnMsg
    else
        tRtn[Protocol_RTN] = Q_RTN_FAILE
    end
    
    local strMsg = cjson.encode(tRtn)
    g_objSessionManager:sendToCur(strMsg, string.len(strMsg))
end
RegNetEvent(Debug_DoString, lua_DoStr)
