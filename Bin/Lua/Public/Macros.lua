--[[
宏定义
--]]

--无效ID
Q_INVALID_ID = -1

--session状态 与c++中enum SessionStatus对应
SessionStatus = {
    "Closed",
    "Linked",
    "ShakeHands",--已经握手,websock使用
    "Logining",  --登陆中
    "Logined",  --已经登陆
    "Loading",  --加载玩家信息中
    "Loaded",  --加载玩家信息完成
    "Createing",  --创建玩家中
    "Created",  --创建玩家完成
    "Playing",  --游戏中
    "GM",--GM登录
}
SessionStatus = creatEnumTable(SessionStatus, -1)

--消息载体类型
CarrierType = {
    "Json",
    "Protobuf",
}
CarrierType = creatEnumTable(CarrierType, 1)

--设置使用的消息载体类型
MsgCarrier = CarrierType.Protobuf

--服务器类型
SVType = {
    "Account",--账号
    "Payment",--支付
    "Game",   --游戏
    "Fight",  --战斗
    "DataBase",--数据库
}
SVType = creatEnumTable(SVType, 1)

--游戏事件
GameEvent = {
    "Start",--启动
    "Started",--启动完成
    "ShutDown",--关闭
    "FPS",--每帧
    "DayChange",--天变
    "WeekChange",--周变
    "MonthChange",--月变
    "OneSecond",--1秒
    "FiveSecond",--5秒
    "TenSecond",--10秒
    "OneMinute",--1分钟
    "FiveMinute",--5分钟
    "TenMinute",--10分钟
    "OneHour",--1小时
    "Close",--连接关闭
    "HttpRead",--http
}
GameEvent = creatEnumTable(GameEvent, 1)

SessionType = {
    "TcpClient",
    "SVLinker",
    "WebSock",
}
SessionType = creatEnumTable(SessionType)

--文本日志级别
LOGLV_NOLOG  = 0
LOGLV_ERROR  = 300
LOGLV_WARN   = 400
LOGLV_INFO   = 500
LOGLV_DEBUG  = 600
