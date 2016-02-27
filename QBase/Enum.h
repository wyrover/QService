
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

/*aes key����*/
enum AESKeyType
{
    AESKEY_NONE = 0,
    AESKEY_128 = 128,
    AESKEY_192 = 192,
    AESKEY_256 = 256
};

/*��־����,ԽСԽ��*/
enum LOG_LEVEL
{
    LOGLV_NOLOG  = 0,
    LOGLV_ERROR  = 300, 
    LOGLV_WARN   = 400,
    LOGLV_INFO   = 500,
    LOGLV_DEBUG  = 600
};

/*���񼶱�*/
enum TaskLevel
{
    TASKLV_HIGHT = 0,
    TASKLV_NORMAL,
    TASKLV_LOW
};

/*���ݿ�����*/
enum DBType
{
    DBTYPE_SQLITE3 = 0,
    DBTYPE_MYSQL
};

//��������
enum SessionType
{
    STYPE_TCP = 0,//tcp
    STYPE_TCPCLIENT,//tcp����
    STYPE_WEBSOCK,//websocket
    STYPE_DEBUG//DEBUG
};

/*session״̬*/
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

/*����*/
enum RunStatus
{
    RUNSTATUS_UNKNOWN = 0,//δ֪
    RUNSTATUS_ERROR,//��������
    RUNSTATUS_STARTING,//��������
    RUNSTATUS_RUNING,//����
    RUNSTATUS_STOPPING,//����ֹͣ
    RUNSTATUS_STOPPED//ֹͣ
};

/*��������*/
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
    DTYPE_SKIP//��������������
};

/*�ṹ������*/
enum StructAttr
{
    STATTR_NAME = 1,//������
    STATTR_TYPE,//��������
    STATTR_SIZE,//�ֽ� DType_STRING �� DType_SKIP����
    STATTR_STATTR//�ṹ������
};

/*���Ӽ�������*/
enum EncryptType
{
    AES = 1,
    RSA,
    ZLib,
};

#endif//Q_ENUM_H_
