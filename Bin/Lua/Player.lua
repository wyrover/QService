--[[
玩家管理
--]]

--所有玩家数据{id = {}}
g_tAllPlayer
--账号玩家映射{account = {playername = id}
g_tAccountPlayerMap

if not g_tAllPlayer then
    g_tAllPlayer = {}
end

if not g_tAccountPlayerMap then
    g_tAccountPlayerMap = {}
end

--[[
描述：添加玩家
参数：
返回值：无
--]]

--[[
描述：添加玩家
参数：
返回值：无
--]]
function AddPlayer(strAccount, strID, strPlayerName, tInfo)
    if g_tAllPlayer[strID] then
        Debug("AddPlayer player id:"..strID.." already exits.")
        return
    else
        g_tAllPlayer[strID] = tInfo
    end
    
    if not g_tAccountPlayerMap[strAccount] then
        g_tAccountPlayerMap[strAccount] = {}
    end
    
    if not g_tAccountPlayerMap[strAccount][strPlayerName] then
        g_tAccountPlayerMap[strAccount][strPlayerName] = strID
    end
end

--[[
描述：移除玩家
参数：
返回值：无
--]]
function RemovePlayer(strAccount, strID)
    g_tAllPlayer[strID] = nil
    
    if g_tAccountPlayerMap[strAccount] then
        for key, val in pairs(g_tAccountPlayerMap[strAccount]) do
            if val == strID then
                g_tAccountPlayerMap[strAccount][key] = nil
                
                if IsTableEmpty(g_tAccountPlayerMap[strAccount]) then
                    g_tAccountPlayerMap[strAccount] = nil
                end
                
                break
            end
        end
    end
end


