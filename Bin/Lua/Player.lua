--[[
玩家管理
--]]

--所有玩家数据{id = {}}
if not g_tAllPlayer then
    g_tAllPlayer = {}
end

--账号玩家映射{account = {playername = id}
if not g_tAccountPlayerMap then
    g_tAccountPlayerMap = {}
end

--[[
描述：服务器启动成功后调用
参数：
返回值：无
--]]
local function OnSVStartUp()
    Debug("Plyaer OnSVStartUp")
end

local function OnSVShutDown()
    Debug("Plyaer OnSVShutDown")
end

local function OnLinkClose()
    Debug("Plyaer OnLinkClose")
end

--[[
描述：添加玩家
参数：
返回值：无
--]]

