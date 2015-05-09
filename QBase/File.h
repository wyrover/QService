
#ifndef Q_FILE_H_
#define Q_FILE_H_

#include "QString.h"

/*获取文件大小*/
int Q_FileSize(const char *pszFileName, unsigned long &ulSize);
/*判断文件是否存在*/
int Q_FileExist(const char *pszFileName);
/*删除文件*/
int Q_FileDel(const char *pszFileName);
/*文件改名*/
int Q_FileReName(const char *pszOldNam, const char *pszNewNam);

/*返回文件路径*/
int Q_DirName(const char *pszPath, std::string &strPath);
/*判断文件路径是否存在*/
int Q_DirExits(const char *pszPath);
/*获取根目录,windows 为盘符 linux 为/下的子目录*/
int Q_GetDrivce(std::list<std::string> &lstDrivce);
/*创建文件夹*/
int Q_MakeDir(const char *pszPath);
/*创建该路径下所有不存在的文件夹*/
int Q_MakeAllDir(const char *pszPath);
/*获取程序所在路径*/
int Q_GetProPath(std::string &strPath);
/*改变当前工作目录*/
int Q_ChDir(const char *pszPath);
/*删除空目录*/
int Q_DelDir(const char *pszPath);
/*删除目录*/
void Q_DelAllDir(const char *pszPath);

/*获取子目录*/
int Q_GetSubDirName(const char *pszParentPathName, std::list<std::string> &lstDirName);
/*获取目录下所有文件*/
int Q_GetAllFileName(const char *pszParentPathName, std::list<std::string> &lstFileName);

/*检查程序是否已经运行*/
int Q_RunOne(void);
/*文件拷贝*/
int Q_FileCopy(const char *pszOldPath, const char *pszNewPath);

#endif//Q_FILE_H_
