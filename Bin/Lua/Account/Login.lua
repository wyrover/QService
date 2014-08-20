--[[
账号服务器验证
--]]

function CSLogIn(tbMessage)
	local tSALogIn = {}
	tSALogIn[Protocol_Request] = AS_LogIn
	tSALogIn[Protocol_ClientID] = tbMessage[Protocol_ClientID]
	tSALogIn[Protocol_CheckID] = tbMessage[Protocol_CheckID]
	tSALogIn[Protocol_RTN] = 0

	
	
	local strMsg = cjson.encode(tSALogIn)
	Debug("CSLogIn send message "..strMsg.." to game server")
	g_objSessionManager:sendToCur(strMsg, string.len(strMsg))
end
RegNetEvent(SA_LogIn, CSLogIn)
