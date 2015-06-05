
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
    acVal = "this is server!",
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
    "this is server!",
}

local strEmpty = ""

local str125 = ""
for i = 1, 124 do
    str125 = str125 .. "S"
end
str125 = str125 .. "2"

local str65535 = ""
for i = 1, 65534 do
    str65535 = str65535 .. "S"
end
str65535 = str65535 .. "3"

local strBigThan65535 = ""
for i = 1, 65535 do
    strBigThan65535 = strBigThan65535 .. "S"
end
strBigThan65535 = strBigThan65535 .. "4"

local tProBuf = {
    sInt = -123,
    uInt = 1234,
    Str = "this is server!",
}

EchoMsgType = {
    "Json",
    "String",
    "Struct",
    "Protobuf",
    "SInt64",
    "UInt64",
}
EchoMsgType = table.enum(EchoMsgType, 1)

function EchoSV()
    g_objNetBinary:reSetWrite()
    local iType = g_objNetBinary:getUint8()
    g_objNetBinary:setUint8(iType)
    
    if EchoMsgType.Json == iType then
        local strMsg = g_objNetBinary:getString() 
        
        g_objNetBinary:setString(cjson.encode(tJson))        
    end
    if EchoMsgType.String == iType then
        local strMsg = g_objNetBinary:getString()
        
        local iRand = math.random(1, 4)
        if 1 == iRand then
            g_objNetBinary:setString(strEmpty)
        end
        if 2 == iRand then
            g_objNetBinary:setString(str125)
        end
        if 3 == iRand then
            g_objNetBinary:setString(str65535)
        end
        if 4 == iRand then
            g_objNetBinary:setString(strBigThan65535)
        end
    end
    
    if EchoMsgType.Struct == iType then
        local tInfo = g_objNetBinary:getStruct(tStructAttr3)
        
        g_objNetBinary:setStruct(tStruct3Val, tStructAttr3)
    end
    
    if EchoMsgType.Protobuf == iType then
        local iLens = g_objNetBinary:getLens() - 1
        local Byte = g_objNetBinary:getByte(iLens)
        local tInfo = protobuf.decode("Echo.EchoMsg", Byte, iLens)
        
        local strRtn = protobuf.encode("Echo.EchoMsg", tProBuf)
        g_objNetBinary:setByte(strRtn, string.len(strRtn))
    end
    
    if EchoMsgType.SInt64 == iType then
        local strSin64 = g_objNetBinary:getSint64()
        g_objNetBinary:setSint64(strSin64)
    end
    if EchoMsgType.UInt64 == iType then  
        local strUin64 = g_objNetBinary:getUint64()
        g_objNetBinary:setUint64(strUin64)
    end
    
    g_objSessionMgr:sendBToCur()
end
