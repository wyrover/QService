#include "TestFile.h"

void CTestFile::Test_Ini(void)
{
    CIniFile objIni;
    Q_FileCopy("../Bin/Test.ini", "../Bin/Test1.ini");

    objIni.setFile("../Bin/Test1.ini");
    printf("%s\n", objIni.getStringValue("sub", "subb"));
    objIni.delKey("main", "mainc");
    objIni.delNode("sub");
    objIni.delNode("sub2");
    objIni.delNode("sub3");
    objIni.setStringValue("main", "maind", "change main d");

    objIni.Save();
}

void CTestFile::Test_TableFile(void)
{
    CTableFile objTableFile;

    objTableFile.setFile("../Bin/itemstrengthen.txt");
    objTableFile.setSplitFlag("	");

    int iRtn = objTableFile.Parse();
    if (Q_RTN_OK != iRtn)
    {
        CPPUNIT_ASSERT(false);
    }

    std::map<int, std::string>::const_iterator itHead;
    const std::map<int, std::string> *pHead = objTableFile.getHead();
    for (itHead = pHead->begin(); pHead->end() != itHead; itHead++)
    {
        printf("%s	", itHead->second.c_str());
    }

    printf("\n");

    while(!objTableFile.eof())
    {
        printf("%d   %d   %d   %d   %d   %d   %d   %d\n", 
            objTableFile.getIntValue("原始道具类型"),
            objTableFile.getIntValue("原始道具ID"),
            objTableFile.getIntValue("目标道具类型"),
            objTableFile.getIntValue("目标道具ID"),
            objTableFile.getIntValue("冷却时间（秒）"),
            objTableFile.getIntValue("消耗铜钱"),
            objTableFile.getIntValue("限制等级"),
            objTableFile.getIntValue("暴击几率"));

        objTableFile.nextRow();
    }
}
