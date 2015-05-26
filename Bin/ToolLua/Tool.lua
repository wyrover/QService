local luaDir1 = Q_GetModulPath() .. "ToolLua" .. Q_GetPathSeparator()
local luaDir2 = Q_GetModulPath() .. "Lua" .. Q_GetPathSeparator() .. "Public" .. Q_GetPathSeparator()
package.path = package.path .. ";" .. luaDir1 .. "?.lua"
package.path = package.path .. ";" .. luaDir2 .. "?.lua"

require("InitTool")

local tNowDay = os.date("*t", time)
if not g_objBinary then
    g_objBinary = nil
end 
if not g_objSessionMgr then
    g_objSessionMgr = nil
end
if not g_objToolDlg then
    g_objToolDlg = nil
end

--启动
function Lua_onStartUp(objSessionManager, objBinary)
    g_objSessionMgr = objSessionManager
    g_objBinary = objBinary
    math.randomseed(tonumber(tostring(os.time()):reverse():sub(1,6)))
    
    onGameEvent(ClientEvent.Start)--这里一般读取配置文件
    onGameEvent(ClientEvent.Started)--这里读取配置文件完成后做初始化操作
end

--退出
function Lua_onStop()
    onGameEvent(ClientEvent.ShutDown)
end

--创建的N个连接
function Lua_onAssistConn(objSession)
    onGameEvent(ClientEvent.AssistLinked, objSession)
end
function Lua_onAssistRead()
    --local strMsg = "Lua_onAssistRead:" .. string.sub(g_objBinary:getString(), 1, g_objBinary:getLens())
    --showMfcMsg(strMsg, string.len(strMsg))
    --onNetEvent(iProtocol, tbMessage)
    local iID = g_objSessionMgr:getCurSession():getSessionID()
    local strMsg = string.format("client id %d:", iID)
    showMfcMsg(strMsg, string.len(strMsg))
    
    showEcho()
end
function Lua_onAssistClose(objSession)
    onGameEvent(ClientEvent.AssistClose, objSession)
end

--主连接
function Lua_onMainConn(objSession)
    onGameEvent(ClientEvent.MainLinked, objSession)
end
function Lua_onMainRead()
    if not getChecked() then
        local strMsg = string.sub(g_objBinary:getString(), 1, g_objBinary:getLens())
        showMfcMsg(strMsg, string.len(strMsg))
    else
        local iBufferLens = g_objBinary:getLens()
        if iBufferLens < 2 then
            local strMsg = "recv error message."
            showMfcMsg(strMsg, string.len(strMsg))
            return
        end
        
        local iProtocol = g_objBinary:getUint16()
        local strMsg = string.format("protocol %d", iProtocol)
        showMfcMsg(strMsg, string.len(strMsg))
        
        if iBufferLens > 2 then
            local iMsgLens = iBufferLens - 2
            local strMsg = g_objBinary:getByte(iMsgLens)
            local strProro = getProtoStr(iProtocol)
            local tMsg = protobuf.decode(strProro, strMsg, iMsgLens)
            if tMsg then
                local strJson = cjson.encode(tMsg)
                showMfcMsg(strJson, string.len(strJson))
            end
        end
    end
end

--定时触发
function Lua_onTime()
    local uiCount = g_objSessionMgr:getCount()
    local uiClick = g_objSessionMgr:getTimer()
    local uiElapseTime = uiClick * uiCount
    local uiOneSecond = 1000
    
    --每帧处理
    onGameEvent(ClientEvent.FPS, uiClick)
    
    --1秒
    if 0 == (uiElapseTime % uiOneSecond) then
        onDelayEvent()
        onGameEvent(ClientEvent.OneSecond)
        
        --检查变天
        local tDay = os.date("*t", time)
        if (tDay.year ~= tNowDay.year) 
            or (tDay.month ~= tNowDay.month) 
            or (tDay.day ~= tNowDay.day) then
                --变天事件
                tNowDay = tDay
                Debug("day changed.")
                onGameEvent(ClientEvent.DayChange)
                
                local iNewWeek = tonumber(os.date("%U", os.time(tDay)))
                local iOldWeek = tonumber(os.date("%U", os.time(tNowDay)))
                if iNewWeek ~= iOldWeek then
                    onGameEvent(ClientEvent.WeekChange)
                end
                if tDay.month ~= tNowDay.month then
                    onGameEvent(ClientEvent.MonthChange)
                end
                
                tNowDay = tDay
        end
    end
    
    --3秒
    if 0 == (uiElapseTime % (uiOneSecond * 3)) then
        onGameEvent(ClientEvent.ThreeSecond)
    end
    
    --5秒
    if 0 == (uiElapseTime % (uiOneSecond * 5)) then
        onGameEvent(ClientEvent.FiveSecond)
    end
    
    --10秒
    if 0 == (uiElapseTime % (uiOneSecond * 10)) then
        onGameEvent(ClientEvent.TenSecond)
    end
    
    --1分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60)) then
        onGameEvent(ClientEvent.OneMinute)
        local iTotalMem = collectgarbage("count")
        local strMemUsage = string.format("lua memory usage:%d,session size:%d", 
            iTotalMem, g_objSessionMgr:getSessionSize())
        showLuaMemory(strMemUsage, string.len(strMemUsage))
        showMfcMsg(strMemUsage, string.len(strMemUsage))
    end
    
    --3分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 3)) then
        onGameEvent(ClientEvent.ThreeMinute)
    end
    
    --5分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 5)) then
        onGameEvent(ClientEvent.FiveMinute)
    end
    
    --10分钟
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 10)) then
        onGameEvent(ClientEvent.TenMinute)
    end
	
    --1小时
    if 0 == (uiElapseTime % (uiOneSecond * 60 * 60)) then
        onGameEvent(ClientEvent.OneHour)
    end
end

function Lua_setMainParam(objToolDlg, objBinary)
    g_objToolDlg = objToolDlg
    g_objBinary = objBinary
end

--input创建消息 运行在独立的lua vm中 g_objBinary g_objSessionMgr等不可用
function Lua_createMsg(strInput, bDebug)
    if bDebug then
        g_objToolDlg:sendMainMsg(strInput, string.len(strInput))
    else
        local Func, strMsg = load(strInput)
        if Func then
            local bRtn, rtnMsg = callFunc(Func)
            if not bRtn then
                showMfcMsg("error", string.len("error"))
            end
        else
            showMfcMsg(strMsg, string.len(strMsg))
        end
    end
end
