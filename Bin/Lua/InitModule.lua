--[[
加载业务模块
--]]

cjson = require("cjson")

local tLoad = 
{
    {"Public/Macros", true},
    {"Public/OpCodes", true},
    {"Public/ErrorCodes", true},
    {"Public/Utile", true},
    {"Public/RegFuncs", true}, 
    {"Public/Debug", true},
    
    {"GameUtile", true},    
    {"LogIn", true},
    {"Player", true},
}

local function LoadAllFile()
    for _, val in pairs(tLoad) do
        require(val[1])
    end
end

LoadAllFile()

function ReLoad(strFile)
    local luaDir = Q_GetModulPath() .. "Lua" .. Q_GetPathSeparator()
    local strPath = ""
    
    if strFile then
        strPath = luaDir..strFile..".lua"
        Debug("reload file "..strPath)
        dofile(strPath)
        
        return
    end
    
    for _, val in pairs(tLoad) do
        if val[2] then
            strPath = luaDir..val[1]..".lua"
            Debug("reload file "..strPath)
            dofile(strPath)
        end
    end
end
