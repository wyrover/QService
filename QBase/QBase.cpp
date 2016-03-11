
#include "QBase.h"

/*程序退出信号量*/
CQMutex g_objExitMutex;
CCond g_objExitCond;
/*程序所在路径*/
char g_acModulPath[Q_FILEPATH_LENS] = {0};
/*服务器名称*/
std::string g_strServerName;
