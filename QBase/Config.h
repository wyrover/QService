
#ifndef Q_CONFIG_H_
#define Q_CONFIG_H_

/*check system*/
#if defined WIN32
    #define Q_OS_WIN32
#elif defined __sun
    #define Q_OS_SOLARIS
#elif defined __FreeBSD__
    #define Q_OS_FREEBASD
#elif defined __NetBSD__
    #define Q_OS_NETBSD
#elif defined __linux__
    #define Q_OS_LINUX
#elif defined HP_UX_C
    #define Q_OS_HPUX
#elif defined AIX
    #define Q_OS_AIX
#else
    #error unknown os system!
#endif

/*check atomic support*/
#ifdef __GNUC__
    #if (__GNUC__ < 4) || \
            ((__GNUC__ == 4) && ((__GNUC_MINOR__ < 1) || \
            ((__GNUC_MINOR__ == 1) && \
            (__GNUC_PATCHLEVEL__ < 2))))
        #pragma error "gcc 4.1.2 or new requested!"
    #endif
#endif

//#define Q_UUID

#define Q_WITHMYSQL

#endif//Q_CONFIG_H_
