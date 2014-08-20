--[[
请求码定义
--]]

Debug_DoString = 0                     --执行命令
Debug_DoStringRtn = Debug_DoString + 1 --执行命令返回
Server_RegServer = Debug_DoStringRtn + 1 --注册服务器
Server_RegServerRtn = Server_RegServer + 1 --注册服务器返回

CS_LogIn = Server_RegServerRtn + 1       --客户端到游戏服务器登陆请求
SA_LogIn = CS_LogIn + 1                --游戏服务器到账号服务器验证请求
AS_LogIn = SA_LogIn + 1                --账号服务到游戏服务器验证返回
SC_LogIn = AS_LogIn + 1                --游戏服务器返回登陆验证结果
