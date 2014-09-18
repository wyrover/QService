--[[
宏定义
--]]

--无效ID
Q_INVALID_ID = -1

--保存玩家每次发送数量
SavePlayer_PreNum  = 100

--数据库日志类型
DBLogType = {
    "LogIn",
    "LogOut",
    "CreatePlayer",
}
DBLogType = CreatEnumTable(DBLogType)

--session状态 与c++中enum SessionStatus对应
SessionStatus = {
    "Unknown",  --未知
    "Logining",  --登陆中
    "Logined",  --已经登陆
    "Loading",  --加载玩家信息中
    "Loaded",  --加载玩家信息完成
    "Createing",  --创建玩家中
    "Created",  --创建玩家完成
    "Playing",  --游戏中
}
SessionStatus = CreatEnumTable(SessionStatus, -1)

--服务器类型
SVType = {
    "Account",
    "Payment",
    "Game",
    "Fight",
    "DataBase",
}
SVType = CreatEnumTable(SVType)

--游戏事件
GameEvent = {
    "StartUp",
    "ShutDown",

    "FPS",
    "DayChange",

    "OneSecond",
    "FiveSecond",
    "TenSecond",
    "OneMinute",
    "FiveMinute",
    "TenMinute",
    "OneHour",

    "Close",

    "LogIned",
}
GameEvent = CreatEnumTable(GameEvent)

--日志级别
LOGLV_NOLOG  = 0
LOGLV_ERROR  = 300
LOGLV_WARN   = 400
LOGLV_INFO   = 500
LOGLV_DEBUG  = 600
