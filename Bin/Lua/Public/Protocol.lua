--[[
协议编号
--]]

--协议编号
Protocol = {
    "Sys_Debug", --调试
    "Sys_DebugRtn",--调试返回    
    "CS_Ping",   --ping 
    "SC_Ping",   --ping 
}
Protocol = table.enum(Protocol, 1)

--协议对应的protobuf字符串
local tProtoStr = {
    {Protocol.Sys_Debug, "Debug.CSDebug"},
    {Protocol.Sys_DebugRtn, "Debug.SCDebug"},
}

--protobuf文件
local tProtoFile = {
    "Debug.proto",
    "Echo.proto",
}

local function loadProto()
    local strProtoDir = Q_GetModulPath() .. "ProtobufStr"
    parser.register(tProtoFile, strProtoDir)
end

local function initProtoStr()
    for _, val in pairs(tProtoStr) do
        setProtoStr(val[1], val[2])
    end
end

loadProto()
initProtoStr()
