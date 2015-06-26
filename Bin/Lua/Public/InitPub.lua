--[[
公共模块加载 
--]]

cjson = require("cjson")
--系统信息获取
sigar = require("sigar")
--文件系统 
lfs = require("lfs")

--[[
描述：加载lua文件
参数：
返回值： 
--]]
function loadFile(varFile)
    local luaDir = Q_GetModulPath() .. "Lua" .. Q_GetPathSeparator()
    local strPath = ""

    if "table" == type(varFile) then
        for _, val in pairs(varFile) do
            if val[2] then
                strPath = luaDir .. val[1] .. ".lua"
                dofile(strPath)
            else
                require(val[1])
            end
        end
    else
        strPath = luaDir .. varFile .. ".lua"
        dofile(strPath)
    end
end

local tLoad = 
{
    {"Public/pbc/initpbc", false},
    {"Public/luasocket/initluasocket", false},
    {"Public/redis/initredis", false},
    {"Public/CppUtile", true},
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
}

loadFile(tLoad)
