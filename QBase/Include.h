
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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <string>
#include <map>
#include <list>
#include <bitset>
#include <queue>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#ifdef Q_OS_WIN
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
#include "event2/http.h"
#include "event2/http_struct.h"
#include "event2/event.h"
#include "event2/thread.h"
#include "Lua5.2/lua.hpp"
#include "LuaBridge/LuaBridge.h"
extern "C"
{
    #include "curl/curl.h"
}

#endif//Q_INCLUDE_H_
