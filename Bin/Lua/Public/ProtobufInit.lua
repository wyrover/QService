--[[
Protobuf初始化
--]]

--协议对应的protobuf字符串
local tProtoStr = {
    {Protocol.CS_Login, "PK_Login.CS_Login"},
    {Protocol.SC_Login, "PK_RtnCode.RtnCode"},
}

--protobuf文件
local tProtoFile = {
    "Login.proto",
    "ReturnCode.proto",
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
