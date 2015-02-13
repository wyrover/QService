#ifndef EVENT_CONFIG_H_ 
#define EVENT_CONFIG_H_

#ifdef Q_OS_WIN32
#include "event-config-win32.h"
#endif

#ifdef Q_OS_LINUX
#include "event-config-linux.h"
#endif

#endif//EVENT_CONFIG_H_
