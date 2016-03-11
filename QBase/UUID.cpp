
#include "UUID.h"

#ifdef Q_UUID

SINGLETON_INIT(CUUID)
CUUID objUUID;

CUUID::CUUID(void)
{
    Q_Zero(m_acBuf, sizeof(m_acBuf));
#ifdef Q_OS_WIN
    (void)CoInitialize(NULL);
#endif
}

CUUID::~CUUID(void)
{
#ifdef Q_OS_WIN
    CoUninitialize();
#endif
}

int CUUID::createGuid(GUID &stUUID)
{
#ifdef Q_OS_WIN
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
* Return       :const char *
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
const char *CUUID::getUUID(void)
{
    GUID stGuid;
    Q_Zero(m_acBuf, sizeof(m_acBuf));

    if (Q_RTN_OK != createGuid(stGuid))
    {
        return "";
    }

    Q_Snprintf(m_acBuf,
        sizeof(m_acBuf) - 1,
        "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
        (unsigned int)stGuid.Data1, stGuid.Data2, stGuid.Data3,
        stGuid.Data4[0], stGuid.Data4[1],
        stGuid.Data4[2], stGuid.Data4[3],
        stGuid.Data4[4], stGuid.Data4[5],
        stGuid.Data4[6], stGuid.Data4[7]);

    return m_acBuf;
}

#endif//Q_UUID
