--[[
执行命令
--]]

--[[
描述：执行字符串命令
参数：
返回值：无
--]]
local function Sys_DoStr(tbMessage)
    local Func = load(tbMessage[ProtocolStr_DebugStr])
    local tRtn = {}
    tRtn[ProtocolStr_Request] = System_DoStringRtn
    if Func then
        local bRtn, rtnMsg = CallFunc(Func)
        if bRtn then
            tRtn[ProtocolStr_Rtn] = Q_RTN_OK
        else
            tRtn[ProtocolStr_Rtn] = Q_RTN_FAILE
        end
        
        tRtn[ProtocolStr_DebugStrRtn] = rtnMsg
    else
        tRtn[ProtocolStr_Rtn] = Q_RTN_FAILE
    end
    
    local strMsg = cjson.encode(tRtn)
    g_objSessionManager:sendToCur(strMsg, string.len(strMsg))
end
RegNetEvent(System_DoString, Sys_DoStr)
