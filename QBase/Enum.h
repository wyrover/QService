
#ifndef Q_ENUM_H_
#define Q_ENUM_H_

enum  WebSockOpCode
{
    WSOCK_CONTINUATION = 0x00,
    WSOCK_TEXTFRAME = 0x01,
    WSOCK_BINARYFRAME = 0x02,
    WSOCK_CLOSE = 0x08,
    WSOCK_PING = 0x09,
    WSOCK_PONG = 0x0A
};

/*aes key类型*/
enum AESKeyType
{
    AESKEY_NONE = 0,
    AESKEY_128 = 128,
    AESKEY_192 = 192,
    AESKEY_256 = 256
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
    TASKLV_HIGHT = 0,
    TASKLV_NORMAL,
    TASKLV_LOW
};

/*数据库类型*/
enum DBType
{
    DBTYPE_SQLITE3 = 0,
    DBTYPE_MYSQL
};

//连接类型
enum SessionType
{
    STYPE_TCP = 0,//tcp
    STYPE_TCPCLIENT,//tcp连接
    STYPE_WEBSOCK,//websocket
    STYPE_DEBUG//DEBUG
};

/*session状态*/
enum SessionStatus
{
    SESSSTATUS_CLOSED = -1,
    SESSSTATUS_CONNECT,
    SESSSTATUS_LINKED,
    SESSSTATUS_LOGINING,
    SESSSTATUS_LOGINED,
    SESSSTATUS_PLAYING,
    SESSSTATUS_GM
};

/*服务*/
enum RunStatus
{
    RUNSTATUS_UNKNOWN = 0,//未知
    RUNSTATUS_ERROR,//发生错误
    RUNSTATUS_STARTING,//正在启动
    RUNSTATUS_RUNING,//运行
    RUNSTATUS_STOPPING,//正在停止
    RUNSTATUS_STOPPED//停止
};

/*数据类型*/
enum DataType
{
    DTYPE_SINT8 = 0,//char 
    DTYPE_UINT8,//unsigned char
    DTYPE_BOOL,//bool
    DTYPE_SINT16,//short
    DTYPE_UINT16,//unsigned short
    DTYPE_SINT32,//int
    DTYPE_UINT32,//unsigned int
    DTYPE_SINT64,//int64_t 
    DTYPE_UINT64,//uint64_t
    DTYPE_FLOAT,//float
    DTYPE_DOUBLE,//double
    DTYPE_STRING,//string
    DTYPE_BYTE,//byte
    DTYPE_STRUCT,//struct
    DTYPE_SKIP//跳过，控制类型
};

/*结构体属性*/
enum StructAttr
{
    STATTR_NAME = 1,//变量名
    STATTR_TYPE,//数据类型
    STATTR_SIZE,//字节 DType_STRING 和 DType_SKIP有用
    STATTR_STATTR//结构体属性
};

/*链接加密类型*/
enum EncryptType
{
    AES = 1,
    RSA,
    ZLib,
};

#endif//Q_ENUM_H_
