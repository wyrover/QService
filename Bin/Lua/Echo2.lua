--[[
ªÿœ‘
--]]

local Echo2 = {}

local OpCodes = require("OpCodes")
local RegFunc = require("RegFunc")

function Echo2.Lua_Echo(objSessionManager, pszMessage, uiLens)
	print("-------------------Echo2---------------------------")
end

RegFunc.Reg_Func[OpCodes.Q_OPCODE_ECHO2] = Echo2.Lua_Echo

print("register opcode "..OpCodes.Q_OPCODE_ECHO2..",func Echo2.Lua_Echo.")

return Echo2
