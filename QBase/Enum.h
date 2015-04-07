/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 * github:https://github.com/QService/QService
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#ifndef Q_ENUM_H_
#define Q_ENUM_H_

/*aes key类型*/
enum AESKeyType
{
    KeyNone = 0,
    Key128 = 128,
    Key192 = 192,
    Key256 = 256,
};

/*加解密方式*/
enum EncryptType
{
    EncryptType_None = 0,
    EncryptType_AES,
    EncryptType_Base64,
    EncryptType_RSA,

    EncryptType_Count,
};

/*日志级别,越小越高*/
enum LOG_LEVEL
{
    LOGLV_NOLOG  = 0,
    LOGLV_ERROR  = 300, 
    LOGLV_WARN   = 400,
    LOGLV_INFO   = 500,
    LOGLV_DEBUG  = 600,
};

/*任务级别*/
enum TaskLevel
{
    Q_ThreadLV_High = 0,
    Q_ThreadLV_Normal,
    Q_ThreadLV_Low,
};

/*数据库类型*/
enum DBType
{
    DBType_Sqlite3 = 0,
    DBType_MySql,
};

/*事件优先级*/
enum EventPriority
{
    Priority_Hight = 0,
    Priority_Normal,

    Priority_Count,
};

/*session类型*/
enum SessionType
{
    SType_TcpClient = 0,
    SType_SVLinker,
    SType_WebSock,
};

/*session状态*/
enum SessionStatus
{
    SessionStatus_Closed = -1,
    SessionStatus_Connect,
    SessionStatus_Linked,
    SessionStatus_Loging,
    SessionStatus_Logined,
    SessionStatus_Loading,
    SessionStatus_Loaded,
    SessionStatus_Createing,
    SessionStatus_Created,
    SessionStatus_Playing,
    SessionStatus_GM,
};

/*服务*/
enum RunStatus
{
    RunStatus_Unknown = 0,//未知
    RunStatus_Error,//发生错误
    RunStatus_Starting,//正在启动
    RunStatus_Runing,//运行
    RunStatus_Stopping,//正在停止
    RunStatus_Stopped,//停止
};

#endif//Q_ENUM_H_
