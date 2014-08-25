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

#ifndef Q_INCLUDE_H_
#define Q_INCLUDE_H_

#define _CRT_SECURE_NO_DERRECATE

#include "Config.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <fcntl.h>
#include <wchar.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <string>
#include <map>
#include <list>
#include <queue>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#ifdef Q_OS_WIN32
    #pragma warning(disable:4786)
    #pragma warning(disable:4819)
    #pragma warning(disable:4996)
    #pragma warning(disable:4806)
    #pragma warning(disable:4009)
    #pragma warning(disable:4290)
    #include <winsock2.h>
    #include <ws2ipdef.h>
    #include <ws2tcpip.h>
    #include <TlHelp32.h>
    #include <io.h>
    #include <tchar.h>
    #include <direct.h>
    #include <process.h>
    #include <ObjBase.h>
    #include <Windows.h>
    #include <unordered_map>
#else
    #include <pthread.h>
    #include <unistd.h>
    #include <signal.h>
    #include <netinet/in.h>
    #include <errno.h>
    #include <dirent.h>
    #include <libgen.h>    
    #include <dlfcn.h>
    #include <locale.h>
    #include <syslog.h>
    #include <sys/statvfs.h>
    #include <sys/msg.h>
    #include <sys/sem.h>
    #include <sys/ipc.h>
    #include <sys/errno.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/socketvar.h>
    #include <sys/resource.h>
    #include <sys/mman.h>
    #include <sys/time.h>
    #include <sys/wait.h>
    #include <sys/ioctl.h>
    #include <net/if.h>
    #include <sys/syscall.h>
    #include <net/if_arp.h>
    #include <tr1/unordered_map>
#ifdef Q_UUID
    #include <uuid/uuid.h>
#endif
#endif//Q_OS_WIN32

#ifdef Q_OS_HPUX
    #include <dl.h>
#endif

#include "event2/bufferevent.h"
#include "event2/util.h"
#include "event2/buffer.h"
#include "event2/listener.h"
#include "event2/event.h"
#include "lua5.2/lua.hpp"

#endif//Q_INCLUDE_H_
