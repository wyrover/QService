--[[
Protobuf ������ʼ��
--]]

--Э���Ӧ��protobuf�����ַ���  ��Ҫ�Լ�д��Ӧ
local tProtoDesp = {
    {Protocol.SYS_RegLinkOther, "SS_Link"},
    
    {Protocol.CS_Login, "CS_Login"},
    {Protocol.SC_Login, "SC_RtnCode"},
}

--protobuf�����ļ�����
local function loadProtoFile()
    local strProtoDir = string.format("%s%s%s%s%s%s", 
        Q_GetModulPath(), "Lua", Q_GetPathSeparator(), "Public", Q_GetPathSeparator(),
        "ProtobufDesp")
        
    local tProtoFile = getAllFile(strProtoDir, "proto")
    parser.register(tProtoFile, strProtoDir)
end

local function initProtoDesp()
    for _, val in pairs(tProtoDesp) do
        setMsgDesp(val[1], val[2])
    end
end

if MSGType.Protobuf == MSGCarrier then
    loadProtoFile()
    initProtoDesp()
end
