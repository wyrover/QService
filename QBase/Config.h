
#ifndef Q_CONFIG_H_
#define Q_CONFIG_H_

/*check system*/
#if defined _WIN32
    #define Q_OS_WIN
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

/*check x64 or x86*/
#ifdef Q_OS_WIN
    #ifdef _WIN64
        #define Q_X64
    #else
        #define Q_X86
    #endif
#else
    #ifdef __GNUC__
        #if __x86_64__ || __ppc64__ || __x86_64 || __amd64__  || __amd64
            #define Q_X64
        #else
            #define Q_X86
        #endif
    #else
        #error unknown compile!
    #endif
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

#define Q_UUID

#define Q_WITHMYSQL

#ifdef Q_OS_WIN
//#define Q_IOCP //开启有问题
#endif

#endif//Q_CONFIG_H_
