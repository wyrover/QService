--[[
��lua
--]]

--·�����
local luaDir = string.format("%s%s%s", 
    Q_GetModulPath(), "Lua", Q_GetPathSeparator())
package.path = string.format("%s;%s?.lua", 
    package.path, luaDir)

require("Public/InitPub")
require("Game/InitModule")

local tNowDay = os.date("*t", time)
local iProLens = 2

--session���� CSessionManager
if not g_objSessionMgr then
    g_objSessionMgr = nil
end
--�����ƽ��� CLuaBinary
if not g_objNetBinary then
    g_objNetBinary = nil
end
--���л�ʹ�� CLuaBinary
if not g_objSerBinary then
    g_objSerBinary = nil
end
--����� CEncrypt
if not g_objEncrypt then
    g_objEncrypt = nil
end
--�ʼ�
if not g_objMail then
    g_objMail = Mail:new()
end
--�Ƿ��������ӣ�����״̬���˲�����ĺ���
if not g_ProFilterFun then
    g_ProFilterFun = nil
end
--����Ƿ��������
if not g_Started then
    g_Started = false
end

--[[
���������������ɹ������
������
����ֵ�� ��
--]]
function Lua_OnStartUp(objSessionMgr, objNetBinary, objSerBinary, objEncrypt)
    g_objSessionMgr = objSessionMgr
    g_objNetBinary = objNetBinary
    g_objSerBinary = objSerBinary
    g_objEncrypt = objEncrypt

    math.randomseed(tonumber(tostring(os.time()):reverse():sub(1,6)))
    
    onGameEvent(GameEvent.Start)--����һ���ȡ�����ļ�
    onGameEvent(GameEvent.Started)--�����ȡ�����ļ���ɺ�����ʼ������
end

--[[
����������ر�ʱ����
������
����ֵ����
--]]
function Lua_OnShutDown()
    onGameEvent(GameEvent.ShutDown)
end

--[[
�������������¼�
������
����ֵ����
--]]
function Lua_OnConnected(objSession)
    onGameEvent(GameEvent.OnConnected, objSession)
end

--[[
�������ɶ��¼�
������
����ֵ����
--]]
function Lua_OnSockRead()    
    --buffer�ܳ���
    local iBufferLens = g_objNetBinary:getLens()
    if iBufferLens < iProLens then
        return
    end
    
    --ǰ2�ֽڲ�����    
    local iProtocol = g_objNetBinary:getUint16()
    Debug("protocol %d", iProtocol)
    
    --ȡ��Ϣ
    local tMsg = {}
    if iBufferLens > iProLens then
        local iMsgLens = iBufferLens - iProLens
        
        tMsg = parseMsg(iProtocol, iMsgLens)
    end

    table.print(tMsg)
    
    onNetEvent(iProtocol, tMsg)
end

--[[
����������
������
����ֵ����
--]]
function Lua_OnDebug()
    onGameEvent(GameEvent.Debug, g_objNetBinary:getByte(g_objNetBinary:getLens()))
end

--[[
���������ӶϿ�ʱ����
������
����ֵ����
--]]
function Lua_OnClose(objSession)
    onGameEvent(GameEvent.Close, objSession)
end

--[[
������HTTP�¼�
������
����ֵ����
--]]
function Lua_OnHttpRead(objHttpBuffer)
    onGameEvent(GameEvent.HttpRead, objHttpBuffer)
end

--[[
�����������������ӳɹ�ʱ����
������
����ֵ����
--]]
function Lua_OnLinkedOther(objSession)
    onGameEvent(GameEvent.LinkedOther, objSession)
end

--[[
��������ʱ������
������
����ֵ����
--]]
function Lua_OnTimer()    
    --ÿ֡����
    onGameEvent(GameEvent.FPS, uiClick)
    
    --1��
    if 0 == ((g_objSessionMgr:getCount() * g_objSessionMgr:getTimer()) % 1000) then 
        onDelayEvent()
    end
end
