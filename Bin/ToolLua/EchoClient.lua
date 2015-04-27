local tStruct1Attr = {
    {'cVal', DataType.SINT8},
    {'', DataType.SKIP, 1},
    {'sVal', DataType.SINT16},
    {'iVal', DataType.SINT32},
    {'bVal', DataType.BOOL},
    {'', DataType.SKIP, 3},
    {'fVal', DataType.FLOAT},
    {'dVal', DataType.DOUBLE},
    {'acVal', DataType.STRING, 100},
    {'iVal2', DataType.SINT32},
}
local tStruct2Attr = {
    {'iVal', DataType.SINT32},
    {'', DataType.SKIP, 4},
    {'stTest1', DataType.STRUCT, 0, tStruct1Attr},
}

local tStructAttr3 = {
    {'iVal', DataType.SINT32},
    {'', DataType.SKIP, 4},
    {'stTest2', DataType.STRUCT, 0, tStruct2Attr},
}

local tStruct1Val = {
    cVal = 1,
    sVal = 2,
    iVal = 3,
    bVal = true,
    fVal = 4.1,
    dVal = 5.2,
    acVal = "this is client!",
    iVal2 = 6,
}

local tStruct2Val = {
    iVal = 20,
    stTest1 = tStruct1Val,
}

local tStruct3Val = {
    iVal = 30,
    stTest2 = tStruct2Val,
}

local tJson = {
    1,
    2,
    3,
    "this is client!",
}

local strEmpty = ""

local str125 = ""
for i = 1, 124 do
    str125 = str125 .. "C"
end
str125 = str125 .. "2"

local str65535 = ""
for i = 1, 65534 do
    str65535 = str65535 .. "C"
end
str65535 = str65535 .. "3"

local strBigThan65535 = ""
for i = 1, 65535 do
    strBigThan65535 = strBigThan65535 .. "C"
end
strBigThan65535 = strBigThan65535 .. "4"

local tProBuf = {
    sInt = -123,
    uInt = 1234,
    Str = "this is client!",
}

EchoMsgType = {
    "Json",
    "String",
    "Struct",
    "Protobuf",
    "SInt64",
    "UInt64",
    
    "Count",
}
EchoMsgType = table.enum(EchoMsgType, 1)

function showEcho()
    local iType = g_objBinary:getUint8()    
    if EchoMsgType.Json == iType then
        local strMsg = "message type: Json"
        showMfcMsg(strMsg, string.len(strMsg))
        strMsg = g_objBinary:getString()
        showMfcMsg(strMsg, string.len(strMsg))      
    end
    if EchoMsgType.String == iType then
        local strMsg = "message type: String"
        showMfcMsg(strMsg, string.len(strMsg))
        strMsg = g_objBinary:getString()
        strMsg = "string lens:"..tostring(string.len(strMsg))
        showMfcMsg(strMsg, string.len(strMsg))
    end
    
    if EchoMsgType.Struct == iType then
        local strMsg = "message type: Struct"
        showMfcMsg(strMsg, string.len(strMsg))
        local tInfo = g_objBinary:getStruct(tStructAttr3)
        strMsg = cjson.encode(tInfo)
        showMfcMsg(strMsg, string.len(strMsg))
    end
    
    if EchoMsgType.Protobuf == iType then
        local strMsg =  "message type: Protobuf"
        showMfcMsg(strMsg, string.len(strMsg))
        local iLens = g_objBinary:getLens() - 1
        local Byte = g_objBinary:getByte(iLens)
        local tInfo = protobuf.decode("Echo.EchoMsg", Byte, iLens)
        strMsg = cjson.encode(tInfo)
        showMfcMsg(strMsg, string.len(strMsg))
    end
    
    if EchoMsgType.SInt64 == iType then
        local strMsg =  "message type: SInt64"
        showMfcMsg(strMsg, string.len(strMsg))
        strMsg = g_objBinary:getSint64()        
        showMfcMsg(strMsg, string.len(strMsg))
    end
    
    if EchoMsgType.UInt64 == iType then
        local strMsg =  "message type: UInt64"
        showMfcMsg(strMsg, string.len(strMsg))
        strMsg = g_objBinary:getUint64()        
        showMfcMsg(strMsg, string.len(strMsg))
    end
end

local function EchoClient(iClientID)
    if not g_objSessionMgr:getSessionByID(iClientID) then
        return
    end
    
    g_objBinary:reSetWrite()        
    local iType = math.random(1, EchoMsgType.Count - 1)        
    g_objBinary:setUint8(iType)
        
    if EchoMsgType.Json == iType then
        local strMsg = cjson.encode(tJson)
        g_objBinary:setString(strMsg)
    end
    if EchoMsgType.String == iType then
        local iRand = math.random(1, 4)
        if 1 == iRand then
            g_objBinary:setString(strEmpty)
        end
        if 2 == iRand then
            g_objBinary:setString(str125)
        end
        if 3 == iRand then
            g_objBinary:setString(str65535)
        end
        if 4 == iRand then
            g_objBinary:setString(strBigThan65535)
        end
    end
    if EchoMsgType.Struct == iType then
        g_objBinary:setStruct(tStruct3Val, tStructAttr3)
    end
    if EchoMsgType.Protobuf == iType then
        local strRtn = protobuf.encode("Echo.EchoMsg", tProBuf)
        g_objBinary:setByte(strRtn, string.len(strRtn))
    end
    
    if EchoMsgType.SInt64 == iType then
        g_objBinary:setSint64("5996902289472770055")
    end
    
    if EchoMsgType.UInt64 == iType then
        local iRand = math.random(1,2)
        local strNumber = "0"
        if iRand == 1 then
            strNumber = "5996902341083680779"
        else
            strNumber = "18446744073709551615"
        end
        
        g_objBinary:setUint64(strNumber)
    end
    
    g_objSessionMgr:sendBToByID(iClientID)
    
    local iRandTime = math.random(1, 60)
    regDelayEvent(iRandTime, EchoClient, iClientID)
end

local function onConn(objSession)
    local iID = objSession:getSessionID()    
    local iRandTime = math.random(1, 60)
    regDelayEvent(iRandTime, EchoClient, iID)
end
regGameEvent(ClientEvent.AssistLinked, onConn)
