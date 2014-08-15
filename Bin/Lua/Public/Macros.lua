--[[
宏定义
--]]

local Macros = {}

--服务器类型
Macros.SVType_Game = 0
Macros.SVType_Account = 1
Macros.SVType_Payment = 2
Macros.SVType_Fight = 3

--最大用户数
Macros.MaxClintNum = 2000

--延迟关闭时间
Macros.DelayCloseTime = 2

--日志级别
Macros.LOGLV_NOLOG  = 0
Macros.LOGLV_ERROR  = 300
Macros.LOGLV_WARN   = 400
Macros.LOGLV_INFO   = 500
Macros.LOGLV_DEBUG  = 600

--连接状态
Macros.LinkStatus_Invalid = -1
Macros.LinkStatus_LogIned = 0

--游戏事件
Macros.GameEvent_StartUp = 0
Macros.GameEvent_ShutDown = Macros.GameEvent_StartUp + 1

Macros.GameEvent_FPS = Macros.GameEvent_ShutDown + 1
Macros.GameEvent_DayChange = Macros.GameEvent_FPS + 1
Macros.GameEvent_1Second = Macros.GameEvent_DayChange + 1
Macros.GameEvent_5Second = Macros.GameEvent_1Second + 1
Macros.GameEvent_10Second = Macros.GameEvent_5Second + 1
Macros.GameEvent_1Minute = Macros.GameEvent_10Second + 1
Macros.GameEvent_5Minute = Macros.GameEvent_1Minute + 1
Macros.GameEvent_10Minute = Macros.GameEvent_5Minute + 1

Macros.GameEvent_LogOut = Macros.GameEvent_10Minute + 1

--协议字符
Macros.Protocol_Request = "rq" --请求码
Macros.Protocol_Account = "acc" --账号
Macros.Protocol_RTN = "rtn" --返回码
Macros.Protocol_PSW = "psw" --密码
Macros.Protocol_ClientID = "cid" --客户端session id
Macros.Protocol_CheckID = "ckid" --验证id

return Macros
