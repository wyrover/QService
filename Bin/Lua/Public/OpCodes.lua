--[[
请求码定义
--]]

local OpCodes = {}

OpCodes.Invalid = -1

OpCodes.CS_LogIn = 1                    --客户端到游戏服务器登陆请求
OpCodes.SA_LogIn = OpCodes.CS_LogIn + 1 --游戏服务器到账号服务器验证请求
OpCodes.AS_LogIn = OpCodes.SA_LogIn + 1 --账号服务到游戏服务器验证返回
OpCodes.SC_LogIn = OpCodes.AS_LogIn + 1 --游戏服务器返回登陆验证结果


return OpCodes
