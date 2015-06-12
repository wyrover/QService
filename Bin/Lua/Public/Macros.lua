--[[
宏定义
--]]

--无效ID
Q_INVALID_ID = -1
Q_INVALID_SOCK = -1

--通信消息类型
MSGType = {
    "Protobuf",
    "Json",
    "Struct",
}
MSGType = table.enum(MSGType, 1)
--当前使用的类型
MSGCarrier = MSGType.Protobuf

--session状态 与c++中enum SessionStatus对应
SessionStatus = {
    "Closed",
    "Connect",
    "Linked",
    "Logining",  --登陆中
    "Logined",  --已经登陆
    "Playing",  --游戏中
    "GM",--GM登录
}
SessionStatus = table.enum(SessionStatus, -1)

--游戏事件
GameEvent = {
    "Start",--启动
    "Started",--启动完成
    "ShutDown",--关闭
    "FPS",--每帧
    "Close",--连接关闭
    "HttpRead",--http
    "LinkedOther",--服务器间连接成功
    "OnConnected",--连接成功
    "Debug",--调试
}
GameEvent = table.enum(GameEvent, 1)

--连接类型 与c++SessionType对应
SessionType = {
    "Tcp",
    "TcpClient",
    "WebSock",
    "Debug",
}
SessionType = table.enum(SessionType)

--数据类型 与c++ DataType对应
DataType = {
    "SINT8",--char 
    "UINT8",--unsigned char
    "BOOL",--bool
    "SINT16", --short
    "UINT16",--unsigned short
    "SINT32",--int
    "UINT32",--unsigned int
    "SINT64",--int64_t 
    "UINT64",--uint64_t
    "FLOAT",--float
    "DOUBLE",--double
    "STRING",--string
    "BYTE",--byte
    "STRUCT",--struct
    "SKIP",--跳过，控制类型 字节对齐使用
}
DataType = table.enum(DataType)

--结构体属性 与c++ StructAttr对应
StructAttr = {
    "Name",--变量名
    "Type",--数据类型
    "Size",--字节 DType_STRING 和 DType_SKIP有用
    "StAttr",--结构体属性
}
StructAttr = table.enum(StructAttr, 1)

--文本日志级别
LOGLV_NOLOG  = 0
LOGLV_ERROR  = 300
LOGLV_WARN   = 400
LOGLV_INFO   = 500
LOGLV_DEBUG  = 600
