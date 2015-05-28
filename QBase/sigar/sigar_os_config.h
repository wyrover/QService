
#ifndef SIGAR_OS_CONFIG_H_
#define SIGAR_OS_CONFIG_H_

#if defined _WIN32
#include "win32/sigar_os.h"
#elif defined __sun
#include "solaris/sigar_os.h"
#elif defined __FreeBSD__
#elif defined __linux__
#include "linux/sigar_os.h"
#elif defined HP_UX_C
#include "hpux/sigar_os.h"
#elif defined AIX
#include "aix/sigar_os.h"
#else
#error unknown os system!
#endif

#endif//SIGAR_OS_CONFIG_H_
