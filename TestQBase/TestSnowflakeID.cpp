#include "TestSnowflakeID.h"

void CTestSnowflakeID::Test_SnowflakeID(void)
{
    CSnowflakeID objID;
    int64_t iID = 0;
    CTimer objTimer;
    std::map<int64_t, char> mapID;
    std::map<int64_t, char>::iterator itmapID;

    objID.setMachineID(10);
    for (int i = 0; i < 10000; i++)
    {
        iID = objID.Generate();
        itmapID = mapID.find(iID);
        if (mapID.end() != itmapID)
        {
            CPPUNIT_ASSERT(false);
        }
        else
        {
            mapID[iID] = 0;
        }
    }

    const int iCount = 100000;
    objTimer.reStart();
    for (int i = 0; i < iCount; i++)
    {
        objID.Generate();
    }
    std::cout << "SnowflakeID time:" << objTimer.Elapsed() << std::endl;

    /*CUUID objUUID;
    objTimer.reStart();
    for (int i = 0; i < iCount; i++)
    {
        objUUID.getUUID();
    }
    std::cout << "UUID time:" << objTimer.Elapsed() << std::endl;*/

    CPPUNIT_ASSERT(true);
}
