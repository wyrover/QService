--[[
登陆流程
--]]

--[[
描述：游戏服务器请求登陆
参数：
返回值： 无
--]]
local function SALogIn(tbMessage)    
    --构造数据    
    local tSALogIn = {}
    tSALogIn[ProtocolStr_Request] = AS_LogIn
    tSALogIn[ProtocolStr_Rtn] = 0
    tSALogIn[ProtocolStr_ClientID] = tbMessage[ProtocolStr_ClientID]
    tSALogIn[ProtocolStr_CheckID] = tbMessage[ProtocolStr_CheckID]
    
    local strMsg = cjson.encode(tSALogIn)
    Debug("SALogIn send message "..strMsg.." to game server")
    
    g_objSessionManager:sendToCur(strMsg, string.len(strMsg))
end
RegNetEvent(SA_LogIn, SALogIn)
