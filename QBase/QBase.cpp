
#include "QBase.h"

/*�����˳��ź���*/
CQMutex g_objExitMutex;
CCond g_objExitCond;
/*��������·��*/
char g_acModulPath[Q_FILEPATH_LENS] = {0};
/*����������*/
std::string g_strServerName;
