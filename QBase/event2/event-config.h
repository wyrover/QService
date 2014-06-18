#ifndef EVENT_CONFIG_H_ 
#define EVENT_CONFIG_H_

#ifdef Q_OS_WIN32
#include "event-config-win32.h"
#endif

#ifdef Q_OS_SOLARIS
#include "event-config-solaris.h"
#endif

#ifdef Q_OS_FREEBASD
#include "event-config-freebsd.h"
#endif

#ifdef Q_OS_LINUX
#include "event-config-linux.h"
#endif

#ifdef Q_OS_AIX
#include "event-config-aix.h"
#endif

#ifdef Q_OS_HPUX
#include "event-config-hpux.h"
#endif


#endif//EVENT_CONFIG_H_
