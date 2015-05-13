
#ifndef Q_ENUM_H_
#define Q_ENUM_H_

enum  WebSockOpCode
{
    CONTINUATION = 0x00,
    TEXT_FRAME = 0x01,
    BINARY_FRAME = 0x02,
    CLOSE = 0x08,
    PING = 0x09,
    PONG = 0x0A
};

/*aes key类型*/
enum AESKeyType
{
    KeyNone = 0,
    Key128 = 128,
    Key192 = 192,
    Key256 = 256
};

/*日志级别,越小越高*/
enum LOG_LEVEL
{
    LOGLV_NOLOG  = 0,
    LOGLV_ERROR  = 300, 
    LOGLV_WARN   = 400,
    LOGLV_INFO   = 500,
    LOGLV_DEBUG  = 600
};

/*任务级别*/
enum TaskLevel
{
    Q_ThreadLV_High = 0,
    Q_ThreadLV_Normal,
    Q_ThreadLV_Low
};

/*数据库类型*/
enum DBType
{
    DBType_Sqlite3 = 0,
    DBType_MySql
};

//连接类型
enum SessionType
{
    STYPE_TCP = 0,//tcp
    STYPE_TCPCLIENT,//tcp连接
    STYPE_WEBSOCK//websocket
};

/*session状态*/
enum SessionStatus
{
    SessionStatus_Closed = -1,
    SessionStatus_Connect,
    SessionStatus_Linked,
    SessionStatus_Logining,
    SessionStatus_Logined,
    SessionStatus_Playing,
    SessionStatus_GM
};

/*服务*/
enum RunStatus
{
    RunStatus_Unknown = 0,//未知
    RunStatus_Error,//发生错误
    RunStatus_Starting,//正在启动
    RunStatus_Runing,//运行
    RunStatus_Stopping,//正在停止
    RunStatus_Stopped//停止
};

/*数据类型*/
enum DataType
{
    DType_SINT8 = 0,//char 
    DType_UINT8,//unsigned char
    DType_BOOL,//bool
    DType_SINT16,//short
    DType_UINT16,//unsigned short
    DType_SINT32,//int
    DType_UINT32,//unsigned int
    DType_SINT64,//int64_t 
    DType_UINT64,//uint64_t
    DType_FLOAT,//float
    DType_DOUBLE,//double
    DType_STRING,//string
    DType_BYTE,//byte
    DType_STRUCT,//struct
    DType_SKIP//跳过，控制类型
};

/*结构体属性*/
enum StructAttr
{
    StAttr_Name = 1,//变量名
    StAttr_Type,//数据类型
    StAttr_Size,//字节 DType_STRING 和 DType_SKIP有用
    StAttr_StAttr//结构体属性
};

#endif//Q_ENUM_H_
