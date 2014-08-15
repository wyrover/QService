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

--[[
描述：关闭
参数：
返回值： 无
--]]
function CloseLink(objSessionManager, iClentID)
    objSessionManager:closeLinkByID(iClentID)
end
