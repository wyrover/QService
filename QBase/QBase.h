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

#ifndef Q_QBASE_H_
#define Q_QBASE_H_

/*包含的头*/
#include "Cond.h"
#include "CoreDump.h"
#include "EventBuffer.h"
#include "Exception.h"
#include "InitSock.h"
#include "Library.h"
#include "Mutex.h"
#include "NETAddr.h"
#include "OSFunc.h"
#include "QTime.h"
#include "RWLock.h"
#include "SockPair.h"
#include "SockPairEventParam.h"
#include "SockPairEvent.h"
#include "Thread.h"
#include "ThreadPool.h"
#include "Timer.h"
#include "Server.h"
#include "ServerLinker.h"
#include "QMySQL.h"
#include "Sqlite.h"
#include "DBPool.h"
#include "md5.h"
#include "QRSA.h"
#include "Base64.h"
#include "AES.h"
#include "File.h"
#include "Ini.h"
#include "TableFile.h"
#include "Log.h"
#include "SampleLoger.h"
#include "SysLog.h"
#include "QString.h"
#include "TimerTask.h"
#include "UUID.h"
#include "pugixml/pugixml.hpp"
#include "LuaFunc.h"
#include "Filter.h"
#include "SnowflakeID.h"
#include "MMap.h"
#include "Charset.h"

using namespace pugi;


/*一些全局变量*/

/*日志全局变量*/
extern class CSampleLoger *g_pSampleLoger;
extern Q_SOCK g_SampleLogerFD;
#define Q_LOG(emLogLV, acFormat, ...)  \
    g_pSampleLoger->writeLog(emLogLV, __FILE__, __FUNCTION__, __LINE__, \
    g_SampleLogerFD, acFormat, ##__VA_ARGS__)

/*程序退出信号量*/
extern CMutex g_objExitMutex;
extern CCond g_objExitCond;

/*程序所在路径*/
extern char g_acModulPath[Q_FILEPATH_LENS];

/*服务器ID*/
extern int g_iServerID;

#endif//Q_QBASE_H_
