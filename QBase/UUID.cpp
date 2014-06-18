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

#include "UUID.h"

#ifdef Q_UUID

CUUID::CUUID(void)
{
#ifdef Q_OS_WIN32
    (void)CoInitialize(NULL);
#endif
}

CUUID::~CUUID(void)
{
#ifdef Q_OS_WIN32
    CoUninitialize();
#endif
}

int CUUID::createGuid(GUID &stUUID)
{
#ifdef Q_OS_WIN32
    if (S_OK != CoCreateGuid(&stUUID))
    {
        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
#else
    uuid_generate(reinterpret_cast<unsigned char *>(&stUUID));

    return Q_RTN_OK;
#endif
}

/************************************************************************
* Function name:getUUID
* Description  :ªÒ»°“ªUUID
* IN           :NONE
* OUT          :NONE
* Return       :std::string
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
std::string CUUID::getUUID(void)
{
    GUID stGuid;
    char acBuf[Q_UUIDLENS] = {0};

    if (Q_RTN_OK != createGuid(stGuid))
    {
        return "";
    }

    Q_Snprintf(acBuf,
        sizeof(acBuf) - 1,
        "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
        (unsigned int)stGuid.Data1, stGuid.Data2, stGuid.Data3,
        stGuid.Data4[0], stGuid.Data4[1],
        stGuid.Data4[2], stGuid.Data4[3],
        stGuid.Data4[4], stGuid.Data4[5],
        stGuid.Data4[6], stGuid.Data4[7]);

    return std::string(acBuf);
}

#endif//Q_UUID
