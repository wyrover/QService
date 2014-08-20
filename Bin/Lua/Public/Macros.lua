--[[
宏定义
--]]

--服务器类型
SVType_Game = 0
SVType_Account = 1
SVType_Payment = 2
SVType_Fight = 3

--最大用户数
MaxClintNum = 2000

--延迟关闭时间
DelayCloseTime = 2
--登陆超时时间
LoginTimeOut = 10

--日志级别
LOGLV_NOLOG  = 0
LOGLV_ERROR  = 300
LOGLV_WARN   = 400
LOGLV_INFO   = 500
LOGLV_DEBUG  = 600

--连接状态
LinkStatus_Invalid = -1
LinkStatus_LogIning = 0
LinkStatus_LogIned = 1

--游戏事件
GameEvent_StartUp = 0
GameEvent_ShutDown = GameEvent_StartUp + 1

GameEvent_FPS = GameEvent_ShutDown + 1
GameEvent_DayChange = GameEvent_FPS + 1
GameEvent_1Second = GameEvent_DayChange + 1
GameEvent_5Second = GameEvent_1Second + 1
GameEvent_10Second = GameEvent_5Second + 1
GameEvent_1Minute = GameEvent_10Second + 1
GameEvent_5Minute = GameEvent_1Minute + 1
GameEvent_10Minute = GameEvent_5Minute + 1

GameEvent_LogOut = GameEvent_10Minute + 1

--协议字符
Protocol_Request = "rq" --请求码
Protocol_Account = "acc" --账号
Protocol_RTN = "rtn" --返回码
Protocol_PSW = "psw" --密码
Protocol_ClientID = "cid" --客户端session id
Protocol_CheckID = "ckid" --验证id
Protocol_DebugStr = "dstr" --命令字符
Protocol_DebugStrRtn = "drtnstr" --命令字符执行返回
Protocol_ServerID = "svid" --服务器ID
