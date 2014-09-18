--[[
协议编号,协议字符
--]]

--协议编号
Protocol = {
    "System_DoString", --执行命令
    "System_DoStringRtn",--执行命令返回
    "System_RegSV",--服务器间注册
    "System_RegSVRtn",--服务器间注册返回

    "CS_Ping",--ping
    "CS_LogIn",--客户端请求登陆
    "SA_LogIn",--游戏服务器向账号服务器发送验证请求
    "AS_LogIn",--账号服务器返回验证请求
    "SC_LogIn",--向客户端返回验证信息
    "DB_LoadPlayer",--请求加载玩家信息
    "SC_AccountInfo",--账号角色信息
    "CS_CreatPlayer",--创建角色
    "DB_CreatPlayer",--数据库创建角色
    "SC_CreatPlayer",--创建角色返回
    "CS_SelectPlayer",--选择角色进入游戏
    "SC_SelectPlayer",--选择角色进入游戏返回
    "DB_Log",--数据库日志
    "DB_SavePlayer",--保存玩家信息
    "SC_KickPlayer",--剔除玩家
}
Protocol = CreatEnumTable(Protocol)

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
