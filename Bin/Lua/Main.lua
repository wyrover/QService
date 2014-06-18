--[[
主lua
--]]

--路径添加
local luaDir = Q_GetModulPath() .. "Lua" .. Q_GetPathSeparator()
package.path = package.path .. ";" .. luaDir .. "?.lua"

require("InitModule")
local RegFunc = require("RegFunc")

--[[
描述：服务启动成功后调用
参数：
返回值： 无
--]]
function Lua_OnStartUp(objSessionManager)
	print("Lua_OnStartUp")
end

--[[
描述：服务关闭时调用
参数：
返回值：无
--]]
function Lua_OnShutDown(objSessionManager)
	print("Lua_OnShutDown")
end

--[[
描述：连接可读事件
参数：
返回值：无
--]]
function Lua_OnRead(objSessionManager, pszMessage, uiLens)
	local iOpCode = 1
	local Func = RegFunc.Reg_Func[iOpCode]
	if Func then
		Func(objSessionManager, pszMessage, uiLens)
	else
	    print("unknown opcode " .. iOpCode .." close this link.")
		objSessionManager:closeCurClint()
	end
end

--[[
描述：定时器调用
参数：
返回值：无
--]]
function Lua_OnTimer(objSessionManager)
	print("Lua_OnTimer count "..objSessionManager:getCount().."  timer "..objSessionManager:getTimer())
	local strMsg = "this is timer.."
	objSessionManager:sendToAll(strMsg, string.len(strMsg))
end

--[[
描述：连接断开时调用
参数：
返回值：无
--]]
function Lua_OnClose(objSessionManager)
	print("Lua_OnClose")
end

--[[
描述：服务器间连接成功时调用
参数：
返回值：无
--]]
function Lua_OnLinkedServer(objSessionManager, objSession)
	print("Lua_OnLinkedServer")
end
