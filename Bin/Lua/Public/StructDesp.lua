--[[
struct ������ʼ�� 
--]]

--struct�����ļ�����
local function loadStructFile()
    local strStructDir = string.format("%s%s%s%s%s%s", 
        Q_GetModulPath(), "Lua", Q_GetPathSeparator(), "Public", Q_GetPathSeparator(),
        "StructDesp")
        
    local tStructFile = getAllFile(strStructDir, "lua")
    for _, val in pairs(tStructFile) do
        local strReqFile = string.format("Public/StructDesp/%s", 
            string.sub(val, 1, string.len(val) - 4))
        require(strReqFile)
    end
end

if MSGType.Struct == MSGCarrier then
    loadStructFile()
end

--Э���Ӧ��struct����table  ��Ҫ�Լ�д��Ӧ
local tStructDesp = {
    {Protocol.SYS_RegLinkOther, SS_Link},
    
    {Protocol.CS_Login, CS_Login},
    {Protocol.SC_Login, SC_RtnCode},    
}

local function initStructDesp()
    for _, val in pairs(tStructDesp) do
        setMsgDesp(val[1], val[2])
    end
end

if MSGType.Struct == MSGCarrier then
    initStructDesp()
end
