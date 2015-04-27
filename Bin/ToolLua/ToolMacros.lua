--游戏事件
ClientEvent = {
    "Start",--启动
    "Started",--启动完成
    "ShutDown",--关闭
    "FPS",--每帧
    "DayChange",--天变
    "WeekChange",--周变
    "MonthChange",--月变
    "OneSecond",--1秒
    "ThreeSecond",--3秒
    "FiveSecond",--5秒
    "TenSecond",--10秒
    "OneMinute",--1分钟
    "ThreeMinute",--3分钟
    "FiveMinute",--5分钟
    "TenMinute",--10分钟
    "OneHour",--1小时
    "AssistLinked",--连接成功
    "AssistClose",
    "MainLinked",--连接成功
    "CreateMsg",--创建消息
}
ClientEvent = table.enum(ClientEvent, 1)