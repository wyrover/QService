--[[
宏定义
--]]

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

--日志级别
LOGLV_NOLOG  = 0
LOGLV_ERROR  = 300
LOGLV_WARN   = 400
LOGLV_INFO   = 500
LOGLV_DEBUG  = 600
