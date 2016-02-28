
#ifndef Q_QBASE_H_
#define Q_QBASE_H_

/*������ͷ*/
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
#include "SockPairEvent.h"
#include "Thread.h"
#include "ThreadPool.h"
#include "Timer.h"
#include "Server.h"
#include "QMySQL.h"
#include "Sqlite.h"
#include "md5.h"
#include "SHA1.h"
#include "QRSA.h"
#include "Base64.h"
#include "AES.h"
#include "File.h"
#include "Ini.h"
#include "TableFile.h"
#include "Log.h"
#include "DBLoger.h"
#include "TxtLoger.h"
#include "QString.h"
#include "UUID.h"
#include "pugixml/pugixml.hpp"
#include "LuaFunc.h"
#include "QFilter.h"
#include "SnowflakeID.h"
#include "MMap.h"
#include "Charset.h"
#include "HttpClient.h"
#include "HttpParser.h"
#include "Binary.h"
#include "format.h"
#include "Session.h"
#include "SessionManager.h"
#include "LinkOther.h"
#include "ObjectPool.h"
#include "Singleton.h"
#include "Encrypt.h"
#include "CommEncrypt.h"
#include "MailSender.h"
#include "LockThis.h"

using namespace pugi;


/*һЩȫ�ֱ���*/

/*��־ȫ�ֱ���*/
#define Q_LOG(emLogLV, acFormat, ...)  \
    CTxtLoger::getSingletonPtr()->writeLog(emLogLV, __FILE__, __FUNCTION__, __LINE__, \
        acFormat, ##__VA_ARGS__)

/*���ݿ���־*/
#define Q_DBLOG(pszPlayerID, usType, pszMsg, iLens) \
    CDBLoger::getSingletonPtr()->writeDBLog(pszPlayerID, usType, pszMsg, iLens)

/*��������·��*/
extern char g_acModulPath[Q_FILEPATH_LENS];

/*������ID*/
extern int g_iServerID;
/*����������*/
extern std::string g_strServerName;

#endif//Q_QBASE_H_
