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

#ifndef Q_MMAP_H_
#define Q_MMAP_H_

#include "Macros.h"

/*
内存映射
*/
template <typename T>
class CMMap
{
public:
    /*pszFileName --映射文件名 pszMapName --映射名  iNum --数量*/
    CMMap(const char *pszFileName, const char *pszMapName, const size_t iNum);
    ~CMMap(void);

    /*返回T指针*/
    T *getData(void);
    /*返回T数量*/
    size_t getNumber(void);

    const char *getMapName(void);
    const char *getFile(void);

private:
    CMMap(void);

private:
    size_t m_iNum;
    size_t m_iTotalSize;
    void *m_pDate;
    std::string m_strFile;
    std::string m_strMapName;
#ifdef Q_OS_WIN32
    HANDLE m_hMapFile;    
#endif
};

template <typename T>
CMMap<T>::CMMap(const char *pszFileName, const char *pszMapName, const size_t iNum) 
    : m_iNum(iNum), m_iTotalSize(sizeof(T) * iNum), m_pDate(NULL)
{
    if (NULL == pszFileName
        || NULL == pszMapName
        || 0 == iNum)
    {
        return;
    }

    m_strFile = pszFileName;
    m_strMapName = pszMapName;

#ifdef Q_OS_WIN32
    HANDLE hFile = NULL;

    hFile = CreateFile(pszFileName,  
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ|FILE_SHARE_WRITE, 
        NULL,
        OPEN_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL);
    if (INVALID_HANDLE_VALUE == hFile)  
    {
        Q_Printf("CreateFile error, code %d.", GetLastError());

        return;
    }

    m_hMapFile = CreateFileMapping((HANDLE)hFile, 
        NULL, 
        PAGE_READWRITE, 
        0,
        m_iTotalSize, 
        m_strMapName.c_str());
    if(NULL == m_hMapFile)
    {
        (void)CloseHandle(hFile);
        Q_Printf("CreateFileMapping error, code %d.", GetLastError());

        return;
    }

    m_pDate = MapViewOfFile(m_hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        m_iTotalSize);
    if (NULL == m_pDate)
    {
        (void)CloseHandle(m_hMapFile);
        (void)CloseHandle(hFile);
        m_hMapFile = NULL;
        Q_Printf("MapViewOfFile error, code %d.", GetLastError());

        return;
    }
#else
    int iFd = -1;

    iFd = open(pszFileName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);  
    if(-1 == iFd)
    {
        Q_Printf("open error, code %d.", errno);

        return;
    }

    if (0 != ftruncate(iFd, m_iTotalSize))
    {
        close(iFd);
        Q_Printf("ftruncate error, code %d.", errno);

        return;
    }

    m_pDate = mmap(NULL, m_iTotalSize, PROT_READ|PROT_WRITE, MAP_SHARED, iFd, 0);
    close(iFd);
    if(MAP_FAILED == m_pDate)  
    {
        Q_Printf("mmap error, code %d.", errno);

        return;
    }
#endif
}

template <typename T>
CMMap<T>::~CMMap(void)
{
#ifdef Q_OS_WIN32
    if (NULL != m_pDate)
    {
        (void)UnmapViewOfFile(m_pDate);
    }
    if (NULL != m_hMapFile)
    {
        (void)CloseHandle(m_hMapFile);
        m_hMapFile = NULL;
    }    
#else
    if (MAP_FAILED != m_pDate
        && NULL != m_pDate)
    {
        (void)munmap(m_pDate, m_iTotalSize);
        m_pDate = MAP_FAILED;
    }
#endif
}

template <typename T>
T * CMMap<T>::getData(void)
{
    return (T*)m_pDate;
}

template <typename T>
size_t CMMap<T>::getNumber(void)
{
    return m_iNum;
}

template <typename T>
const char *CMMap<T>::getMapName(void)
{
    return m_strMapName.c_str();
}

template <typename T>
const char *CMMap<T>::getFile(void)
{
    return m_strFile.c_str();
}

#endif//Q_MMAP_H_
