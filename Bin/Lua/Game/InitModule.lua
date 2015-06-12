--[[
加载业务模块
--]]

cjson = require("cjson")
--系统信息获取
sigar = require("sigar")
--文件系统 
lfs = require("lfs")

local tLoad = 
{
    --公共
    {"Public/pbc/initpbc", false},
    {"Public/luasocket/initluasocket", false},
    {"Public/redis/initredis", false},
    {"Public/Utile", true},
    {"Public/Macros", true},
    {"Public/ErrorCodes", true},
    {"Public/TimeWheel", true},
    {"Public/RegFuncs", true},
    {"Public/Protocol", true},
    {"Public/ProtobufDesp", true},
    {"Public/StructDesp", true},
    {"Public/NetMsgParser", true}, 
    {"Public/SessionUtile", true}, 
    {"Public/Mail", true},
    {"Public/Debug", false},--这个设为真，重加载会循环加载      
    {"Public/LogLuaMemory", true},
    {"Public/Ping", true},    
    {"Public/Object", true},
    {"Public/Component", true},
    {"Public/Component/InitComp", true},
    {"Public/Entity", true},
    {"Public/Entity/InitEntity", true},
    
    
    {"Game/LogIn", true},
    
    {"Game/Test", true},
}

local function loadAllFile()
    for _, val in pairs(tLoad) do
        require(val[1])
    end
end

loadAllFile()

--[[
描述：重加载lua文件
参数：strFile --文件 为nil则加载tLoad中的所有
返回值： 无
--]]
function reLoad(strFile)
    local tTmp = nil
    
    if strFile then
        tTmp = {strFile, true}
    else
        tTmp = tLoad
    end
    
    loadFile(tTmp)
end
