
#include "QBase.h"

/*系统日志*/
CSysLoger g_SysLoger;

/*日志全局变量*/
CTxtLoger *g_pTxtLoger = NULL;
Q_SOCK g_TxtLogerFD = Q_INVALID_SOCK;

/*数据库日志*/
class CDBLoger *g_pDBLoger = NULL;
Q_SOCK g_DBLogerFD = Q_INVALID_SOCK;

/*程序退出信号量*/
CQMutex g_objExitMutex;
CCond g_objExitCond;
/*程序所在路径*/
char g_acModulPath[Q_FILEPATH_LENS] = {0};
/*服务器ID*/
int g_iServerID = 0;
/*服务器名称*/
std::string g_strServerName;
