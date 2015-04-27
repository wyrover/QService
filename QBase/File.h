/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 * github:https://github.com/QService/QService
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this std::list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this std::list of conditions and the following disclaimer in the
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
