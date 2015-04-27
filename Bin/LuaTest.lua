function table.enum(tMsg, iBegin) 
    assert("table" == type(tMsg))    
    local tEnum = {} 
    local iEnumIndex = iBegin or 0 
    for key, val in pairs(tMsg) do 
        tEnum[val] = iEnumIndex + key - 1
    end 
    
    return tEnum 
end 

function table.print(lua_table, indent)
    assert("table" == type(lua_table))    
    indent = indent or 0
    for k, v in pairs(lua_table) do
        if type(k) == "string" then
            k = string.format("%q", k)
        end
        
        local szSuffix = ""
        if type(v) == "table" then
            szSuffix = "{"
        end
        
        local szPrefix = string.rep("    ", indent)
        formatting = szPrefix.."["..k.."]".." = "..szSuffix
        
        if type(v) == "table" then
            print(formatting)
            table.print(v, indent + 1)
            print(szPrefix.."},")
        else
            local szValue = ""
            if type(v) == "string" then
                szValue = string.format("%q", v)
            else
                szValue = tostring(v)
            end
            
            print(formatting..szValue..",")
        end
    end
end

--数据类型 与c++ DataType对应
DataType = {
    "SINT8",--char 
    "UINT8",--unsigned char
    "BOOL",--bool
    "SINT16", --short
    "UINT16",--unsigned short
    "SINT32",--int
    "UINT32",--unsigned int
    "SINT64",--int64_t 
    "UINT64",--uint64_t
    "FLOAT",--float
    "DOUBLE",--double
    "STRING",--string
    "BYTE",--byte
    "STRUCT",--struct
    "SKIP",--跳过，控制类型 字节对齐使用
}
DataType = table.enum(DataType)

local iType = DataType.STRING

--[[
struct Test1
{
    char cVal;
    short sVal;
    int iVal;
    bool bVal;
    float fVal;
    double dVal;
    char acVal[100];
    int iVal2;
    Test1(void)
    {
        Q_Zero(acVal, sizeof(acVal));
    };
};
struct Test2
{
    int iVal;
    Test1 stTest1;
};
struct Test3
{
    int iVal;
    Test2 stTest2;
};

--]]

--struct Test1 write
function TestStruct1(objBinary)
    objBinary:reSetWrite()
    local tStructVal = {
        cVal = 1,
        sVal = 2,
        iVal = 3,
        bVal = true,
        fVal = 4.1,
        dVal = 5.2,
        acVal = "this is test!",
        iVal2 = 6,
    }
    local tStructAttr = {
        {'cVal', DataType.SINT8},
        {'', DataType.SKIP, 1},
        {'sVal', DataType.SINT16},
        {'iVal', DataType.SINT32},
        {'bVal', DataType.BOOL},
        {'', DataType.SKIP, 3},
        {'fVal', DataType.FLOAT},
        {'dVal', DataType.DOUBLE},
        {'acVal', iType, 100},
        {'iVal2', DataType.SINT32},
    }
    
    local bOk = objBinary:setStruct(tStructVal, tStructAttr)
end

--struct Test3 write
function TestStruct2(objBinary)
    objBinary:reSetWrite()
    local tStructVal1 = {
        cVal = 1,
        sVal = 2,
        iVal = 3,
        bVal = true,
        fVal = 4.1,
        dVal = 5.2,
        acVal = "this is test!",
        iVal2 = 6,
    }
    local tStructAttr1 = {
        {'cVal', DataType.SINT8},
        {'', DataType.SKIP, 1},
        {'sVal', DataType.SINT16},
        {'iVal', DataType.SINT32},
        {'bVal', DataType.BOOL},
        {'', DataType.SKIP, 3},
        {'fVal', DataType.FLOAT},
        {'dVal', DataType.DOUBLE},
        {'acVal', iType, 100},
        {'iVal2', DataType.SINT32},
    }
    
    local tStructVal2 = {
        iVal = 20,
        stTest1 = tStructVal1,
    }
    local tStructAttr2 = {
        {'iVal', DataType.SINT32},
        {'', DataType.SKIP, 4},
        {'stTest1', DataType.STRUCT, 0, tStructAttr1},
    }
    local tStructVal3 = {
        iVal = 30,
        stTest2 = tStructVal2,
    }
    local tStructAttr3 = {
        {'iVal', DataType.SINT32},
        {'', DataType.SKIP, 4},
        {'stTest2', DataType.STRUCT, 0, tStructAttr2},
    }
    
    local bOk = objBinary:setStruct(tStructVal3, tStructAttr3)
end

--struct Test1 read
function TestStructRead1(objBinary)
    local tStructAttr = {
        {'cVal', DataType.SINT8},
        {'', DataType.SKIP, 1},
        {'sVal', DataType.SINT16},
        {'iVal', DataType.SINT32},
        {'bVal', DataType.BOOL},
        {'', DataType.SKIP, 3},
        {'fVal', DataType.FLOAT},
        {'dVal', DataType.DOUBLE},
        {'acVal', iType, 100},
        {'iVal2', DataType.SINT32},
    }
    
    local tInfo = objBinary:getStruct(tStructAttr)
    print("----------------------------------")
    table.print(tInfo)
end

--struct Test3 read
function TestStructRead2(objBinary)
    local tStructAttr1 = {
        {'cVal', DataType.SINT8},
        {'', DataType.SKIP, 1},
        {'sVal', DataType.SINT16},
        {'iVal', DataType.SINT32},
        {'bVal', DataType.BOOL},
        {'', DataType.SKIP, 3},
        {'fVal', DataType.FLOAT},
        {'dVal', DataType.DOUBLE},
        {'acVal', iType, 100},
        {'iVal2', DataType.SINT32},
    }
    local tStructAttr2 = {
        {'iVal', DataType.SINT32},
        {'', DataType.SKIP, 4},
        {'stTest1', DataType.STRUCT, 0, tStructAttr1},
    }
    local tStructAttr3 = {
        {'iVal', DataType.SINT32},
        {'', DataType.SKIP, 4},
        {'stTest2', DataType.STRUCT, 0, tStructAttr2},
    }
    local tInfo = objBinary:getStruct(tStructAttr3)
    print("----------------------------------")
    table.print(tInfo)
end
