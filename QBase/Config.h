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
