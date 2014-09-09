--[[
协议编号,协议字符
--]]

--协议编号
System_DoString = 0                                          --执行命令
System_DoStringRtn = System_DoString + 1                     --执行命令返回
System_RegSV = System_DoStringRtn + 1                        --服务器间注册
System_RegSVRtn = System_RegSV + 1                           --服务器间注册返回

CS_Ping = System_RegSVRtn + 1                                --ping
CS_LogIn = CS_Ping + 1                                       --客户端请求登陆
SA_LogIn = CS_LogIn + 1                                      --游戏服务器向账号服务器发送验证请求
AS_LogIn = SA_LogIn + 1                                      --账号服务器返回验证请求
SC_LogIn = AS_LogIn + 1                                      --向客户端返回验证信息
DB_LoadPlayer = SC_LogIn + 1                                 --请求加载玩家信息
SC_AccountInfo = DB_LoadPlayer + 1                           --账号角色信息
CS_CreatPlayer = SC_AccountInfo + 1                          --创建角色
DB_CreatPlayer = CS_CreatPlayer + 1                          --数据库创建角色
SC_CreatPlayer = DB_CreatPlayer + 1                          --创建角色返回
CS_SelectPlayer = SC_CreatPlayer + 1                         --选择角色进入游戏
SC_SelectPlayer = CS_SelectPlayer + 1                        --选择角色进入游戏返回
DB_Log          = SC_SelectPlayer + 1                        --数据库日志
DB_SavePlayer   = DB_Log + 1                                 --保存玩家信息
SC_KickPlayer   = DB_SavePlayer + 1                          --剔除玩家

--协议字符
ProtocolStr_Request     = "rq"                               --请求码
ProtocolStr_Rtn         = "rtn"                              --返回码
ProtocolStr_DebugStr    = "dstr"                             --命令字符
ProtocolStr_DebugStrRtn = "drtnstr"                          --命令字符执行返回
ProtocolStr_ServerID    = "svid"                             --服务器ID
ProtocolStr_Flag        = "flag"                             --标志
ProtocolStr_Account     = "acc"                              --账号
ProtocolStr_PSW         = "psw"                              --密码
ProtocolStr_ID          = "id"                               --ID
ProtocolStr_ClientID    = "cid"                              --连接ID
ProtocolStr_CheckID     = "ckid"                             --校验ID
ProtocolStr_Number      = "num"                              --数量
ProtocolStr_Info        = "info"                             --详细信息
ProtocolStr_Name        = "nam"                              --名称
ProtocolStr_Attr        = "attr"                             --属性

ProtocolStr_Log_Type    = "logtype"                          --日志类型
ProtocolStr_Log_Param0  = "p0"                               --日志参数0
ProtocolStr_Log_Param1  = "p1"                               --日志参数1
ProtocolStr_Log_Param2  = "p2"                               --日志参数2
ProtocolStr_Log_Param3  = "p3"                               --日志参数3
ProtocolStr_Log_Param4  = "p4"                               --日志参数4
ProtocolStr_Log_Memo    = "memo"                             --日志description
