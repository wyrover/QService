--[[
游戏常用函数
--]]

--[[
描述：获取玩家连接数
参数：
返回值：无
--]]
function GetOnLineNum()
    return g_objSessionManager:getSessionSize() - g_objSessionManager:getGetSVLinkerNum()
end

--[[
描述：获取在线玩家ID
参数：
返回值： 无
--]]
function GetOnLineID()
    return g_objSessionManager:getOnLineID()
end
