/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 * github:https://github.com/QService/QService
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this std::std::list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this std::std::list of conditions and the following disclaimer in the
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

#include "File.h"

/*************************************************
* Function name:Q_FileSize
* Description  :获取文件大小
* IN           :pszFileName --文件名
* OUT          :lSize --大小
* Return       :
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :first program
**************************************************/
int Q_FileSize(const char *pszFileName, unsigned long &ulSize)
{
    if (NULL == pszFileName)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return Q_ERROR_NULLPOINTER;
    }

    int iRtn = Q_RTN_OK;
    struct Q_STAT stBuffer;

    ulSize = Q_INIT_NUMBER;

    iRtn = Q_STAT(pszFileName, &stBuffer);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    ulSize = stBuffer.st_size;

    return Q_RTN_OK;
}

/*************************************************
* Function name:Q_FileExist
* Description  :判断文件是否存在
* IN           :pszFileName --文件名
* OUT          :None
* Return       :RTN_OK 存在 RTN_FAIL 不存在
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
int Q_FileExist(const char *pszFileName)
{
    if (NULL == pszFileName)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return Q_ERROR_NULLPOINTER;
    }

    return Q_ACCESS(pszFileName, 0);
}

/*************************************************
* Function name:Q_FileDel
* Description  :删除文件
* IN           :pszFileName --文件名
* OUT          :None
* Return       :RTN_OK 成功 RTN_FAIL 失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
int Q_FileDel(const char *pszFileName)
{
    int iRtn = Q_RTN_OK;

    iRtn = Q_FileExist(pszFileName);
    if (Q_RTN_OK != iRtn)
    {
        return Q_RTN_OK;
    }

    iRtn = remove(pszFileName); 
    
    return iRtn;
}

/*************************************************
* Function name:Q_FileReName
* Description  :文件改名
* IN           :pszOldNam --原文件名 pszNewNam --要改成的文件名
* OUT          :None
* Return       :RTN_OK 成功 RTN_FAIL 失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :fist program
**************************************************/
int Q_FileReName(const char *pszOldNam, const char *pszNewNam)
{
    if (NULL == pszOldNam
        || NULL == pszNewNam)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return Q_ERROR_NULLPOINTER;
    }

    int iRtn = Q_RTN_OK;

    iRtn = Q_FileExist(pszOldNam);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    iRtn = rename(pszOldNam, pszNewNam);

    return iRtn;
}

bool Q_File_Access_Read(const char *pszFileName)
{
    if (NULL == pszFileName)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return false;
    }

    if (Q_RTN_OK == Q_ACCESS(pszFileName, 4))
    {
        return true;
    }
    
    return false;
}

bool Q_File_Access_Write(const char *pszFileName)
{
    if (NULL == pszFileName)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return false;
    }

    if (Q_RTN_OK == Q_ACCESS(pszFileName, 2))
    {
        return true;
    }

    return false;
}

/*************************************************
* Function name:Q_DirName
* Description  :返回文件路径
* IN           :pszPath --文件全路径
* OUT          :strPath --目录
* Return       :文件路径
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :first program
**************************************************/
int Q_DirName(const char *pszPath, std::string &strPath)
{
    if (NULL == pszPath)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return Q_ERROR_NULLPOINTER;
    }

    size_t iLens = strlen(pszPath);

    if (iLens < 2
        || iLens >= Q_FILEPATH_LENS)
    {
        return Q_RTN_FAILE;
    }

    strPath.clear();

#ifdef Q_OS_WIN32
    int iPos = Q_INIT_NUMBER;

    strPath = pszPath;
    iPos = strPath.find_last_of(Q_PATH_SEPARATOR);
    if (std::string::npos != iPos)
    {
        strPath = strPath.substr(0, iPos);
    }
#else
    strPath = dirname((char*)pszPath);
#endif 

    return Q_RTN_OK;
}

/*************************************************
* Function name:Q_DirExits
* Description  :判断文件路径是否存在
* IN           :pszPath --文件夹路径
* OUT          :None
* Return       :Q_RTN_OK 成功 其他 失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
int Q_DirExits(const char *pszPath)
{
    if (NULL == pszPath)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return Q_ERROR_NULLPOINTER;
    }

    int iLens = strlen(pszPath);

    if (0 == iLens
        || iLens >= Q_FILEPATH_LENS)
    {
        return Q_RTN_FAILE;
    }    

#ifdef Q_OS_WIN32
    HANDLE pvFile = NULL;
    char acTmpPath[Q_FILEPATH_LENS] = {0};
    WIN32_FIND_DATA stFindFileData;

    memcpy(acTmpPath, pszPath, strlen(pszPath));
    if ('\\' == acTmpPath[strlen(acTmpPath) - 1])
    {
        acTmpPath[strlen(acTmpPath) - 1] = '\0';
    }

    pvFile = FindFirstFile(acTmpPath, &stFindFileData);
    if (INVALID_HANDLE_VALUE == pvFile)
    {
        return Q_RTN_FAILE;
    }

    if (stFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        FindClose(pvFile);

        return Q_RTN_OK;
    }

    FindClose(pvFile);

    return Q_RTN_FAILE;
#else
    DIR *pDir = opendir(pszPath);
    if (NULL != pDir)
    {
        closedir(pDir);

        return Q_RTN_OK;
    }

    return Q_RTN_FAILE;
#endif
}

/*************************************************
* Function name:Q_MakeDir
* Description  :创建文件夹
* IN           :pszPath --文件夹路径
* OUT          :None
* Return       :RTN_OK 成功 其他 失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
int Q_MakeDir(const char *pszPath)
{
    if (NULL == pszPath)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return Q_ERROR_NULLPOINTER;
    }

    if (0 == strlen(pszPath))
    {
        return Q_RTN_FAILE;
    }    

#ifdef Q_OS_WIN32
    return _mkdir(pszPath);
#else
    return mkdir(pszPath, S_IRWXU);
#endif
}

/*************************************************
* Function name:Q_GetDrivce
* Description  :获取根目录,windows 为盘符 linux 为/下的子目录
* IN           :None
* OUT          :lstDrivce --盘符
* Return       :RTN_OK 成功 其他 失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
int Q_GetDrivce(std::list<std::string> &lstDrivce)
{
#ifdef Q_OS_WIN32
    std::string strTmp;
    char acDriverStr[Q_ONEK] = {0};
    int iLen = GetLogicalDriveStrings(sizeof(acDriverStr), acDriverStr);
    if (0 == iLen)
    {
        return Q_Error();
    }

    for(int i = 0; i < iLen; i++)
    {
        if (NULL == acDriverStr[i])
        {
            if (!strTmp.empty())
            {
                lstDrivce.push_back(strTmp);
            }

            strTmp.clear();
        }
        else
        {
            strTmp.push_back(acDriverStr[i]);
        }
    }

    return Q_RTN_OK;
#else
    return Q_GetSubDirName(Q_PATH_SEPARATOR, lstDrivce);
#endif
}

/*************************************************
* Function name:Q_MakeAllDir
* Description  :创建该路径下所有不存在的文件夹
* IN           :pszPath --文件夹路径
* OUT          :None
* Return       :RTN_OK 成功 其他 失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
int Q_MakeAllDir(const char *pszPath)
{
    if (NULL == pszPath)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return Q_ERROR_NULLPOINTER;
    }

    if (0 == strlen(pszPath))
    {
        Q_Printf("%s", "path is empty");

        return Q_RTN_FAILE;
    }

    int iRtn = Q_RTN_OK;
    int iCount  = Q_INIT_NUMBER;
    std::string strTmp;
    std::string strPath;
    std::list<std::string> lstTmp;    
    std::list<std::string>::iterator itDir;

    //判断是否存在
    iRtn = Q_DirExits(pszPath);
    if (Q_RTN_OK == iRtn)
    {
        return Q_RTN_OK;
    }

    strPath = pszPath;
    Q_Split(strPath, std::string(Q_PATH_SEPARATOR), lstTmp);
    if (lstTmp.empty())
    {
        Q_Printf("%s", "Invalid path");

        return Q_RTN_FAILE;
    }
 
    for (itDir = lstTmp.begin(); lstTmp.end() != itDir; itDir++)
    {
        iCount++;
        strTmp += (*itDir + std::string(Q_PATH_SEPARATOR));
        if (1 == iCount)
        {
            continue;
        }

        iRtn = Q_DirExits(strTmp.c_str());
        if (Q_RTN_OK != iRtn)
        {
            iRtn = Q_MakeDir(strTmp.c_str());
            if (Q_RTN_OK != iRtn)
            {
                return iRtn;
            }
        }
    }

    return Q_RTN_OK;
}

int Q_DelDir(const char *pszPath)
{
#ifdef Q_OS_WIN32
    return _rmdir(pszPath);
#else
    return rmdir(pszPath);
#endif
}

void Q_DelAllDir(const char *pszPath)
{
#ifdef Q_OS_WIN32
    SHFILEOPSTRUCT stFileOP;    
    char acTmpPath[Q_FILEPATH_LENS] = {0};

    memcpy(acTmpPath, pszPath, strlen(pszPath));
    if ('\\' == acTmpPath[strlen(acTmpPath) - 1])
    {
        acTmpPath[strlen(acTmpPath) - 1] = '\0';
    }

    stFileOP.hwnd = NULL;
    stFileOP.wFunc = FO_DELETE;
    stFileOP.pFrom = acTmpPath;
    stFileOP.pTo = NULL;
    stFileOP.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;

    SHFileOperation(&stFileOP);
#else
    std::string strCmd;
    strCmd = "rm -rf " + std::string(pszPath);

    system(strCmd.c_str());
#endif
}

/*************************************************
* Function name:Q_GetProPath
* Description  :获取程序所在路径
* IN           :strPath --路径
* OUT          :None
* Return       :RTN_OK 成功 其他 失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
int Q_GetProPath(std::string &strPath)
{
    int iSize = 0;
    char acPath[Q_FILEPATH_LENS] = {0};

#ifdef Q_OS_WIN32 
    iSize = GetModuleFileName(NULL, acPath, sizeof(acPath) - 1);   
#else
    iSize = readlink("/proc/self/exe", acPath, sizeof(acPath) - 1);
#endif
    if (0 >= iSize 
        || sizeof(acPath) <= (size_t)iSize)
    {
        return Q_RTN_FAILE;
    }

    acPath[iSize] = '\0';

    if (Q_RTN_OK != Q_DirName(acPath, strPath))
    {
        return Q_RTN_FAILE;
    }

    strPath += std::string(Q_PATH_SEPARATOR);

    return Q_RTN_OK;
}

int Q_ChDir(const char *pszPath)
{
#ifdef Q_OS_WIN32
    return _chdir(pszPath);
#else
    return chdir(pszPath);
#endif
}

/*************************************************
* Function name:Q_GetSubDirName
* Description  :获取子目录
* IN           :pszParentPathName --文件夹路径
* OUT          :lstDirName --所有子目录
* Return       :RTN_OK 成功 其他 失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
int Q_GetSubDirName(const char *pszParentPathName, std::list<std::string> &lstDirName)
{
    lstDirName.clear();

#ifdef Q_OS_WIN32
    WIN32_FIND_DATA fd = {0};
    HANDLE hSearch;
    std::string strFilePathName;
    strFilePathName = pszParentPathName + std::string("\\*");

    hSearch = FindFirstFile(strFilePathName.c_str(), &fd);
    if (INVALID_HANDLE_VALUE == hSearch)
    {
        return Q_RTN_FAILE;
    }

    if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
    {
        if (strcmp(fd.cFileName, ".")
            &&strcmp(fd.cFileName, ".."))
        {
            lstDirName.push_back(fd.cFileName);
        }
    }

    for (;;)
    {
        memset(&fd, 0, sizeof(fd));
        if (!FindNextFile(hSearch, &fd))
        {
            if (ERROR_NO_MORE_FILES == GetLastError())
            {
                break;
            }

            FindClose(hSearch);

            return Q_RTN_FAILE;
        }

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
            && !(fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
            && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
            && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
        {
            if (strcmp(fd.cFileName, ".")
                &&strcmp(fd.cFileName, ".."))
            {
                lstDirName.push_back(fd.cFileName);
            }
        }
    }

    FindClose(hSearch);

    return Q_RTN_OK;
#else
    DIR *dir;
    struct dirent *ptr;
    struct stat strFileInfo = {0};
    char acFullName[Q_FILEPATH_LENS] = {0};
    dir = opendir(pszParentPathName);
    if (NULL == dir)
    {
        return Q_RTN_FAILE;
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        Q_Snprintf(acFullName, sizeof(acFullName) - 1, "%s/%s", pszParentPathName, ptr->d_name);
        if (lstat(acFullName, &strFileInfo) < 0)
        {
            closedir(dir);

            return Q_RTN_FAILE;
        }

        if (S_ISDIR(strFileInfo.st_mode))
        {
            if (strcmp(ptr->d_name, ".")
                && strcmp(ptr->d_name, ".."))
            {
                lstDirName.push_back(ptr->d_name);
            }
        }
    }

    closedir(dir);

    return Q_RTN_OK;
#endif
}

/*************************************************
* Function name:Q_GetAllFileName
* Description  :获取目录下所有文件
* IN           :pszParentPathName --文件夹路径
* OUT          :lstFileName --所有文件名
* Return       :RTN_OK 成功 其他 失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/02
* Modification 
* ......record :first program
**************************************************/
int Q_GetAllFileName(const char *pszParentPathName, std::list<std::string> &lstFileName)
{
    lstFileName.clear();

#ifdef Q_OS_WIN32
    WIN32_FIND_DATA fd = {0};
    HANDLE hSearch;
    std::string strFilePathName;
    strFilePathName = pszParentPathName + std::string("\\*");

    hSearch = FindFirstFile(strFilePathName.c_str(), &fd);
    if (INVALID_HANDLE_VALUE == hSearch)
    {
        return Q_RTN_FAILE;
    }

    if (!fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
    {
        if (strcmp(fd.cFileName, ".")
            &&strcmp(fd.cFileName, ".."))
        {
            lstFileName.push_back(fd.cFileName);
        }
    }

    for (;;)
    {
        memset(&fd, 0, sizeof(fd));
        if (!FindNextFile(hSearch, &fd))
        {
            if (ERROR_NO_MORE_FILES == GetLastError())
            {
                break;
            }

            FindClose(hSearch);

            return Q_RTN_FAILE;
        }

        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            && !(fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
            && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
            && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
        {
            if (strcmp(fd.cFileName, ".")
                &&strcmp(fd.cFileName, ".."))
            {
                lstFileName.push_back(fd.cFileName);
            }
        }
    }

    FindClose(hSearch);

    return Q_RTN_OK;
#else
    DIR *dir;
    struct dirent *ptr;
    struct stat strFileInfo = {0};
    char acFullName[Q_FILEPATH_LENS] = {0};

    dir = opendir(pszParentPathName);
    if (NULL == dir)
    {
        return Q_RTN_FAILE;
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        Q_Snprintf(acFullName, sizeof(acFullName) - 1, "%s/%s", pszParentPathName, ptr->d_name);
        if (lstat(acFullName, &strFileInfo) < 0)
        {
            closedir(dir);

            return Q_RTN_FAILE;
        }

        if (!S_ISDIR(strFileInfo.st_mode))
        {
            if (strcmp(ptr->d_name, ".")
                && strcmp(ptr->d_name, ".."))
            {
                lstFileName.push_back(ptr->d_name);
            }
        }
    }

    closedir(dir);

    return Q_RTN_OK;
#endif
}

#ifdef Q_OS_WIN32
#define PIDFILE "342cfb6a-1374-11e2-a327-dd69792138b9"

/*************************************************
* Function name:Q_RunOne
* Description  :检查程序是否已经运行
* IN           :None
* OUT          :None
* Return       :RTN_OK --没有实例运行 其他--已经运行,或者失败
* Make By      :lqf/200309129@163.com
* Date Time    :2012/11/02
* Modification 
* ......record :first program
**************************************************/
int Q_RunOne(void)
{
    ::CreateMutex(NULL, FALSE, _T(PIDFILE)); /*创建一个互斥体*/  
    if(ERROR_ALREADY_EXISTS == GetLastError())   
    {
        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}
#else
#define PIDFILE    ".pid"
#define FILE_MODE  (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

static int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type = type;                /* F_RDLCK, F_WRLCK, F_UNLCK */
    lock.l_start = offset;        /* byte offset, relative to l_whence */
    lock.l_whence = whence;        /* SEEK_SET, SEEK_CUR, SEEK_END */
    lock.l_len = len;                /* #bytes (0 means to EOF) */

    return( fcntl(fd, cmd, &lock) );
}

#define read_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLK, F_RDLCK, offset, whence, len)
#define readw_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLKW, F_RDLCK, offset, whence, len)
#define write_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len)
#define writew_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLKW, F_WRLCK, offset, whence, len)
#define un_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLK, F_UNLCK, offset, whence, len)

/*************************************************
* Function name:Q_RunOne
* Description  :检查程序是否已经运行
* IN           :None
* OUT          :None
* Return       :RTN_OK --没有实例运行 其他--已经运行,或者失败
* Make By      :lqf/200309129@163.com
* Date Time    :2012/11/02
* Modification 
* ......record :first program
**************************************************/
int Q_RunOne(void)
{
    int fd, val;
    char acBuf[10] = {0};

    fd = open(PIDFILE, O_WRONLY | O_CREAT, FILE_MODE);
    if (fd < 0)
    {
        return Q_RTN_FAILE;
    }

    /* try and set a write lock on the entire file */
    if (write_lock(fd, 0, SEEK_SET, 0) < 0)
    {
        return Q_RTN_FAILE;
    }

    /* truncate to zero length, now that we have the lock */
    if (ftruncate(fd, 0) < 0)
    {
        return Q_RTN_FAILE;
    }

    /* and write our process ID */
    Q_Snprintf(acBuf, sizeof(acBuf) - 1, "%d", getpid());
    (void)write(fd, acBuf, strlen(acBuf));
    /* set close-on-exec flag for descriptor */
    if ((val = fcntl(fd, F_GETFD, 0)) < 0)
    {
        return Q_RTN_FAILE;
    }

    val |= FD_CLOEXEC;
    if (fcntl(fd, F_SETFD, val) < 0)
    {
        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}
#endif

/*************************************************
* Function name:Q_FileCopy
* Description  :文件拷贝
* IN           :pszOldPath --源文件路径 pszNewPath --目标文件路径 
* OUT          :None
* Return       :RTN_OK 成功 RTN_FAIL 失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/01
* Modification 
* ......record :first program
**************************************************/
int Q_FileCopy(const char *pszOldPath, const char *pszNewPath)
{
    if (Q_RTN_OK != Q_FileExist(pszOldPath))
    {
        return Q_RTN_FAILE;
    }

    if (Q_RTN_OK != Q_FileDel(pszNewPath))
    {
        return Q_RTN_FAILE;
    }

    FILE *pInFile = NULL;
    FILE *pOutFile = NULL;

    pInFile = fopen(pszOldPath, "rb");
    if (NULL == pInFile)
    {
        return Q_RTN_FAILE;
    }

    pOutFile = fopen(pszNewPath, "wb");
    if (NULL == pInFile)
    {
        fclose(pInFile);

        return Q_RTN_FAILE;
    }

    char acBuf[4096];
    size_t iReadSize = 0;
    size_t iWriteSize = 0;

    while(!feof(pInFile))
    {
        iReadSize = fread(acBuf, 1, sizeof(acBuf), pInFile);
        iWriteSize = fwrite(acBuf, 1, iReadSize, pOutFile);
        if (iReadSize != iWriteSize)
        {
            fclose(pInFile);
            fclose(pOutFile);

            return Q_RTN_FAILE;
        }
    }

    fclose(pInFile);
    fclose(pOutFile);

    return Q_RTN_OK;
}
