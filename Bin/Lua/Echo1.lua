--[[
ªÿœ‘
--]]

local Echo1 = {}

local OpCodes = require("OpCodes")
local RegFunc = require("RegFunc")

function Echo1.Lua_Echo(objSessionManager, pszMessage, uiLens)
	
	local str = "this is from service echo1"
	objSessionManager:sendToCur(str, string.len(str))
	
	local iCurID = objSessionManager:getCurSession():getSessionID()
	print("current session id " .. iCurID)
	
	str = "this is from service pack message."
	objSessionManager:pushMsg(str, string.len(str))
	objSessionManager:sendPushMsgToByID(iCurID)
	objSessionManager:Clear()
end

RegFunc.Reg_Func[OpCodes.Q_OPCODE_ECHO1] = Echo1.Lua_Echo

print("register opcode "..OpCodes.Q_OPCODE_ECHO1..",func Echo1.Lua_Echo.")

return Echo1
