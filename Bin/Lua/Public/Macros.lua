--[[
�궨��
--]]

--��ЧID
Q_INVALID_ID = -1
Q_INVALID_SOCK = -1

--ͨ����Ϣ����
MSGType = {
    "Protobuf",
    "Json",
    "Struct",
}
MSGType = table.enum(MSGType, 1)
--��ǰʹ�õ�����
MSGCarrier = MSGType.Json

--session״̬ ��c++��enum SessionStatus��Ӧ
SessionStatus = {
    "Closed",
    "Connect",
    "Linked",
    "Logining",  --��½��
    "Logined",  --�Ѿ���½
    "Playing",  --��Ϸ��
    "GM",--GM��¼
}
SessionStatus = table.enum(SessionStatus, -1)

--��Ϸ�¼�
GameEvent = {
    "Start",--����
    "Started",--�������
    "ShutDown",--�ر�
    "FPS",--ÿ֡
    "Close",--���ӹر�
    "HttpRead",--http
    "LinkedOther",--�����������ӳɹ�
    "OnConnected",--���ӳɹ�
    "Debug",--����
}
GameEvent = table.enum(GameEvent, 1)

--�������� ��c++SessionType��Ӧ
SessionType = {
    "Tcp",
    "TcpClient",
    "WebSock",
    "Debug",
}
SessionType = table.enum(SessionType)

--�������� ��c++ DataType��Ӧ
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
    "SKIP",--�������������� �ֽڶ���ʹ��
}
DataType = table.enum(DataType)

--�ṹ������ ��c++ StructAttr��Ӧ
StructAttr = {
    "Name",--������
    "Type",--��������
    "Size",--�ֽ� DType_STRING �� DType_SKIP����
    "StAttr",--�ṹ������
}
StructAttr = table.enum(StructAttr, 1)

--�ı���־����
LOGLV_NOLOG  = 0
LOGLV_ERROR  = 300
LOGLV_WARN   = 400
LOGLV_INFO   = 500
LOGLV_DEBUG  = 600
