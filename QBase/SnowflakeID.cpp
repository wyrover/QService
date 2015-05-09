
#include "SnowflakeID.h"

CSnowflakeID::CSnowflakeID(void) : m_iMachineID(Q_INIT_NUMBER), m_iIndex(Q_INIT_NUMBER),
    m_uTime64(Q_INIT_NUMBER), m_uID64(Q_INIT_NUMBER), m_objStream()
{
    evutil_timerclear(&m_stTimeVal);
}

CSnowflakeID::~CSnowflakeID(void)
{

}

uint64_t CSnowflakeID::getTime(void)
{
    m_uTime64 = Q_INIT_NUMBER;
    evutil_timerclear(&m_stTimeVal);
    evutil_gettimeofday(&m_stTimeVal, NULL);

    m_uTime64 = static_cast<uint64_t>(m_stTimeVal.tv_usec) / 1000;//取毫秒
    m_uTime64 += static_cast<uint64_t>(m_stTimeVal.tv_sec) * 1000;

    return m_uTime64;
}

void CSnowflakeID::setMachineID(const int iMachineID)
{
    m_iMachineID = iMachineID;
}

const char *CSnowflakeID::getSnowflakeID(void)
{
    m_strVali64.clear();
    m_objStream.clear();
    m_objStream.str("");

    m_objStream << Generate();

    m_objStream >> m_strVali64;

    return m_strVali64.c_str();
}

uint64_t CSnowflakeID::Generate(void)
{
    m_uID64 = Q_INIT_NUMBER;

    // 保留后41位时间
    m_uID64 = getTime() << 22;
    // 中间10位是机器ID
    m_uID64 |= (m_iMachineID & 0x3FF) << 12;
    // 最后12位是m_iIndex
    m_uID64 |= m_iIndex++ & 0xFFF;
    if (m_iIndex == 0x1000)
    {
        m_iIndex = Q_INIT_NUMBER;
    }

    return m_uID64;
}
