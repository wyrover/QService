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
--加解码
if not g_objEncrypt then
    g_objEncrypt = nil
end

--启动
function Lua_onStartUp(objSessionManager, objBinary, objEncrypt)
    g_objSessionMgr = objSessionManager
    g_objBinary = objBinary
    g_objEncrypt = objEncrypt
    math.randomseed(tonumber(tostring(os.time()):reverse():sub(1,6)))
    
    onGameEvent(GameEvent.Start)--这里一般读取配置文件
    onGameEvent(GameEvent.Started)--这里读取配置文件完成后做初始化操作
end

--退出
function Lua_onStop()
    onGameEvent(GameEvent.ShutDown)
end

--主连接
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
    --每帧处理
    onGameEvent(GameEvent.FPS, uiClick)
    
    --1秒
    if 0 == ((g_objSessionMgr:getCount() * g_objSessionMgr:getTimer()) % 1000) then 
        onDelayEvent()
    end
end

--------------------------------主界面-----------------------------------
function Lua_setMainParam(objToolDlg, objBinary, objEncrypt)
    g_objToolDlg = objToolDlg
    g_objBinary = objBinary
    g_objEncrypt = objEncrypt
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
