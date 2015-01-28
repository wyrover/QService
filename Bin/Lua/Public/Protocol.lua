--[[
协议编号
--]]

--协议编号
Protocol = {
    "Sys_Debug", --调试
    "Sys_DebugRtn",--调试返回
    "Sys_ReqRegSV", --服务器注册
    "Sys_RegSVRtn",--服务器注册返回
    
    "CS_Ping",   --ping 
}
Protocol = creatEnumTable(Protocol, 1)

--protobuf文件
local tProtoFile = {
    "RegSV.proto",
}

--协议对应的protobuf字符串
local tProtoStr = {
    {Protocol.Sys_ReqRegSV, "RegSV.Request"},
    {Protocol.Sys_RegSVRtn, "RegSV.Response"},
}

local function loadProto()
    local strProtoDir = Q_GetModulPath() .. "Lua" .. Q_GetPathSeparator().."Proto"
    parser.register(tProtoFile, strProtoDir)
end

local function initProtoStr()
    for _, val in pairs(tProtoStr) do
        setProtoStr(val[1], val[2])
    end
end

loadProto()
initProtoStr()
