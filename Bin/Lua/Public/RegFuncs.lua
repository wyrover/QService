--[[
函数注册
--]]

local RegFuncs = {}

--网络消息
RegFuncs.NetEvent = {}
--游戏事件
RegFuncs.GameEvent = {}

--[[
描述：游戏事件回调
参数：iEvent --事件编号
返回值：无
--]]
function RegFuncs:OnGameEvent(iEvent, ...)
	if not RegFuncs.GameEvent[iEvent] then
		return
	end
	
	for _, Val in pairs(RegFuncs.GameEvent[iEvent]) do
		local strFunc = Val[1]
		local Func = Val[2]
		
		if Func then
			TimerReStart()
			CallFunc(Func, table.unpack{...})
			Debug("function "..strFunc.." elapsed time:"..tostring(TimerElapsed()).. " ms")
		end
	end
end

--[[
描述：游戏事件注册
参数：
返回值：无
--]]
function RegFuncs:RegGameEvent(iEvent, strFunc, Func)
	if "function" ~= type(Func) then
		return
	end
	
	if not strFunc then
		return
	end
	
	if not RegFuncs.GameEvent[iEvent] then
		RegFuncs.GameEvent[iEvent] = {}
	end
	
	table.insert(RegFuncs.GameEvent[iEvent], {strFunc, Func})
end

--[[
描述：网络可读事件回调
参数：
返回值：无
--]]
function RegFuncs:OnNetEvent(objSessionManager, iOpCode, tbMessage)
	local Func = RegFuncs.NetEvent[iOpCode]
	if Func then
		TimerReStart()
		CallFunc(Func, objSessionManager, tbMessage)
		Debug("protocol "..iOpCode.." elapsed time:"..tostring(TimerElapsed()).. " ms")		
	else
	    Debug("unknown protocol " .. iOpCode ..", close this link.")
		objSessionManager:closeCurClint()
	end
end

--[[
描述：网络可读事件注册
参数：
返回值：无
--]]
function RegFuncs:RegNetEvent(iOpCode, Func)
	if "function" ~= type(Func) then
		return
	end
	
	if RegFuncs.NetEvent[iOpCode] then
		Debug("protocol "..iOpCode.." already register.")
		return
	end
	
	RegFuncs.NetEvent[iOpCode] = Func
	Debug("register protocol "..iOpCode)
end

return RegFuncs
