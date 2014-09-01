--[[
宏定义
--]]

--数据库日志类型
DBLogType_LogIn = 0
DBLogType_LogOut = DBLogType_LogIn + 1
DBLogType_CreatePlayer = DBLogType_LogOut + 1

--无效ID
Q_INVALID_ID = -1

--保存玩家每次发送数量
SavePlayer_PreNum  = 100

--session状态 与c++中enum SessionStatus对应
SessionStatus_Unknown = -1 --未知
SessionStatus_Logining = SessionStatus_Unknown + 1 --登陆中
SessionStatus_Logined = SessionStatus_Logining + 1 --已经登陆
SessionStatus_Loading = SessionStatus_Logined + 1 --加载玩家信息中
SessionStatus_Loaded = SessionStatus_Loading + 1 --加载玩家信息完成
SessionStatus_Createing = SessionStatus_Loaded + 1 --创建玩家中
SessionStatus_Created = SessionStatus_Createing + 1 --创建玩家完成
SessionStatus_Playing = SessionStatus_Created + 1 --游戏中

--服务器类型
SVType_Account = 0
SVType_Payment = SVType_Account + 1
SVType_Game = SVType_Payment + 1
SVType_Fight = SVType_Game + 1
SVType_DataBase = SVType_Fight + 1

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
GameEvent_1Hour = GameEvent_10Minute + 1

GameEvent_Close = GameEvent_1Hour + 1

GameEvent_LogIned = GameEvent_Close + 1

--日志级别
LOGLV_NOLOG  = 0
LOGLV_ERROR  = 300
LOGLV_WARN   = 400
LOGLV_INFO   = 500
LOGLV_DEBUG  = 600
