--[[
网络消息构建  解析 主要在SessionUtile Main中使用 
--]]
 
 --[[
描述：protobuf构建、解析
参数：
返回值：无
--]]
local function createProBuf(iProtocol, tMsg)
    local strProtoDesp = getMsgDesp(iProtocol)
    
    return protobuf.encode(strProtoDesp, tMsg)
end

local function parseProBuf(iProtocol, strMsg, iMsgLens)
    local strProtoDesp = getMsgDesp(iProtocol)
    
    local tMsg = protobuf.decode(strProtoDesp, strMsg, iMsgLens)
    assert(tMsg, protobuf.lasterror())
    
    return tMsg
end

--[[
描述：json构建、解析
参数：
返回值：无
--]]
local function createJsonBuf(tMsg)
    return cjson.encode(tMsg)
end

local function parseJsonBuf(strMsg)    
    return cjson.decode(strMsg)    
end

--[[
描述：struct构建、解析
参数：
返回值：无
--]]
local function createStructBuf(iProtocol, tMsg)
    local tStructDesp = getMsgDesp(iProtocol)
    
    g_objNetBinary:setStruct(tMsg, tStructDesp)
end

local function parseStructBuf(iProtocol)
    local tStructDesp = getMsgDesp(iProtocol)
    
    return g_objNetBinary:getStruct(tStructDesp)
end

--[[
描述：消息构建、解析
参数：
返回值：无
--]]
function createMsg(iProtocol, tMsg)
    g_objNetBinary:reSetWrite()
    
    g_objNetBinary:setUint16(iProtocol)
    
    if (not tMsg) or (table.empty(tMsg)) then
        return
    end
    
    if MSGType.Protobuf == MSGCarrier then
        local strMsg = createProBuf(iProtocol, tMsg)
        g_objNetBinary:setByte(strMsg, string.len(strMsg))
        Debug("create protobuf message.")
    elseif MSGType.Struct == MSGCarrier then
        createStructBuf(iProtocol, tMsg)
        Debug("create struct message.")
    elseif MSGType.Json == MSGCarrier then
        local strMsg = createJsonBuf(tMsg)
        g_objNetBinary:setByte(strMsg, string.len(strMsg))
        Debug("create json message.")
    else
        assert(false)
    end    
end

function parseMsg(iProtocol, iMsgLens)
    local tMsg = {}
    
    if MSGType.Protobuf == MSGCarrier then
        local strMsg = g_objNetBinary:getByte(iMsgLens)
        tMsg = parseProBuf(iProtocol, strMsg, iMsgLens)
        Debug("parse protobuf message.")
    elseif MSGType.Struct == MSGCarrier then
        tMsg = parseStructBuf(iProtocol)
        Debug("parse struct message.")
    elseif MSGType.Json == MSGCarrier then
        local strMsg = g_objNetBinary:getByte(iMsgLens)
        tMsg = parseJsonBuf(strMsg)
        Debug("parse json message.")
    else
        assert(false)
    end
    
    return tMsg
end
