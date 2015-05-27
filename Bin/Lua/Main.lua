--[[
主lua
--]]

--路径添加
local luaDir = string.format("%s%s%s", 
    Q_GetModulPath(), "Lua", Q_GetPathSeparator())
package.path = string.format("%s;%s?.lua", 
    package.path, luaDir)

require("Game/InitModule")

local tNowDay = os.date("*t", time)
local iProLens = 2

--session管理
if not g_objSessionMgr then
    g_objSessionMgr = nil
end
--二进制解析
if not g_objBinary then
    g_objBinary = nil
end
--编解码
if not g_objEncrypt then
    g_objEncrypt = nil
end
--邮件
if not g_objMail then
    g_objMail = Mail:new()
end
--非服务器连接，根据状态过滤操作码的函数
if not g_ProFilterFun then
    g_ProFilterFun = nil
end
--标记是否启动完成
if not g_Started then
    g_Started = false
end

--[[
描述：服务启动成功后调用
参数：
返回值： 无
--]]
function Lua_OnStartUp(objSessionMgr, objBinary, objEncrypt)
    g_objSessionMgr = objSessionMgr
    g_objBinary = objBinary
    g_objEncrypt = objEncrypt
    math.randomseed(tonumber(tostring(os.time()):reverse():sub(1,6)))
    
    onGameEvent(GameEvent.Start)--这里一般读取配置文件
    onGameEvent(GameEvent.Started)--这里读取配置文件完成后做初始化操作
end

--[[
描述：服务关闭时调用
参数：
返回值：无
--]]
function Lua_OnShutDown()
    onGameEvent(GameEvent.ShutDown)
    
    --确认退出，，，没这个不会退出
    shutDown()
end

--[[
描述：新连接事件
参数：
返回值：无
--]]
function Lua_OnConnected(objSession)
    onGameEvent(GameEvent.OnConnected, objSession)
end

--[[
描述：可读事件
参数：
返回值：无
--]]
function Lua_OnSockRead()    
    --buffer总长度
    local iBufferLens = g_objBinary:getLens()
    if iBufferLens < iProLens then
        return
    end
    
    --前2字节操作码    
    local iProtocol = g_objBinary:getUint16()
    Debug("protocol %d", iProtocol)
    
    --取消息
    local tMsg = {}
    if iBufferLens > iProLens then
        local iMsgLens = iBufferLens - iProLens
        local strMsg = g_objBinary:getByte(iMsgLens)
        
        tMsg = parseProBuf(iProtocol, strMsg, iMsgLens)
    end

    table.print(tMsg)
    
    --检查操作码与状态是否匹配
    --[[local objCurSession = g_objSessionMgr:getCurSession()
    if SessionType.Tcp == objCurSession:getType() then
        if not g_Started then
            Debug("service not start completed.")
            return
        end
        
        if g_ProFilterFun then
            local iStatus = objCurSession:getStatus()
            if not g_ProFilterFun(iProtocol, iStatus) then
                Log(LOGLV_WARN, "session status %d, protocol %d was ignored.", 
                    iStatus, iProtocol)
                return
            end
        end
    end--]]
    
    onNetEvent(iProtocol, tMsg)
end

--[[
描述：调试
参数：
返回值：无
--]]
function Lua_OnDebug()
    onGameEvent(GameEvent.Debug, g_objBinary:getByte(g_objBinary:getLens()))
end

--[[
描述：连接断开时调用
参数：
返回值：无
--]]
function Lua_OnClose(objSession)
    onGameEvent(GameEvent.Close, objSession)
end

--[[
描述：HTTP事件
参数：
返回值：无
--]]
function Lua_OnHttpRead(objHttpBuffer)
    onGameEvent(GameEvent.HttpRead, objHttpBuffer)
end

--[[
描述：服务器间连接成功时调用
参数：
返回值：无
--]]
function Lua_OnLinkedOther(objSession)
    onGameEvent(GameEvent.LinkedOther, objSession)
end

--[[
描述：定时器调用
参数：
返回值：无
--]]
function Lua_OnTimer()    
    --每帧处理
    onGameEvent(GameEvent.FPS, uiClick)
    
    --1秒
    if 0 == ((g_objSessionMgr:getCount() * g_objSessionMgr:getTimer()) % 1000) then 
        onDelayEvent()
    end
end
