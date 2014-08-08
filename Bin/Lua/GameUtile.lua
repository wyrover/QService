--[[
游戏常用函数
--]]

--[[
描述：获取玩家连接数
参数：
返回值：无
--]]
function GetOnLineNum(objSessionManager)
	return objSessionManager:getSessionSize() - objSessionManager:getGetSVLinkerNum()
end
