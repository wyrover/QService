
#ifndef Q_SNOWFLAKEID_H_
#define Q_SNOWFLAKEID_H_

#include "Macros.h"

/*
* twitter的snowflake算法
* 64       63--------------22---------12---------0
* 符号位   |     41位时间   |10位机器码|12位自增码|
*/
class CSnowflakeID
{
public:
    CSnowflakeID(void);
    ~CSnowflakeID(void);

    void setMachineID(const int iMachineID);
    uint64_t Generate(void);
    const char *getSnowflakeID(void);

private:
    uint64_t getTime(void);

private:
    int m_iMachineID;
    int m_iIndex;
    uint64_t m_uTime64;
    uint64_t m_uID64;
    struct timeval m_stTimeVal;
    std::string m_strVali64;
    std::stringstream m_objStream;
};

#endif//Q_SNOWFLAKEID_H_
