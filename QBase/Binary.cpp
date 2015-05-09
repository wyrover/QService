
#include "Binary.h"
#include "SysLog.h"
#include "OSFunc.h"
#include "Exception.h"

#define STATTRTAB_MINLENS 2

CLuaBinary::CLuaBinary(void) : m_pParseBuffer(NULL), m_pLua(NULL), 
    m_iParseBufLens(Q_INIT_NUMBER), m_iCurParseLens(Q_INIT_NUMBER),
    m_objWritBuffer()
{
    Q_Zero(acTmp, sizeof(acTmp));
    Q_Zero(acEmpty, sizeof(acEmpty));
}

CLuaBinary::~CLuaBinary(void)
{
    m_pLua = NULL;
    m_pParseBuffer = NULL;
}

void CLuaBinary::setLua(struct lua_State *pLua)
{
    assert(NULL != pLua);

    m_pLua = pLua;
}

void CLuaBinary::setBuffer(const char *pszBuf, const size_t iLens)
{
    assert(NULL != pszBuf);

    m_pParseBuffer = (char*)pszBuf;
    m_iParseBufLens = iLens;
    m_iCurParseLens = Q_INIT_NUMBER;
}

size_t CLuaBinary::getLens(void) const
{
    return m_iParseBufLens;
}

void CLuaBinary::reSetWrite(void)
{
    m_objWritBuffer.reSet();
}

void CLuaBinary::skipRead(const size_t iLens)
{
    m_iCurParseLens += iLens;
}

bool CLuaBinary::skipWrite(const size_t iLens)
{
    if (Q_INIT_NUMBER == iLens)
    {
        return true;
    }

    bool bOk = false;
    if (iLens > sizeof(acTmp))
    {
        char *pTmp = new(std::nothrow) char[iLens];
        if (NULL == pTmp)
        {
            Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);
            return false;
        }

        Q_Zero(pTmp, iLens);
        bOk = setVal(pTmp, iLens);
        Q_SafeDelete_Array(pTmp);
    }
    else
    {
        bOk = setVal(acTmp, iLens);
    }

    return bOk;
}

bool CLuaBinary::setVal(const char *pszBuf, const size_t iLens)
{
    try
    {
        m_objWritBuffer.pushBuff(pszBuf, iLens);
    }
    catch (CQException &e)
    {
        Q_SYSLOG(LOGLV_ERROR, "%s", e.getErrorMsg());

        return false;
    }

    return true;
}

bool CLuaBinary::setSint8(const short sVal)
{
    char cVal = (char)sVal;

    return setVal((const char *)&cVal, sizeof(cVal));
}
short CLuaBinary::getSint8(void)
{
    return readNumber<char>();
}
bool CLuaBinary::setUint8(const unsigned short usVal)
{
    unsigned char ucVal = (unsigned char)usVal;

    return setVal((const char *)&ucVal, sizeof(ucVal));
}
unsigned short CLuaBinary::getUint8(void)
{
    return readNumber<unsigned char>();
}

bool CLuaBinary::setBool(const bool bVal)
{
    unsigned char ucVal = (bVal ? 1 : 0);

    return setVal((const char *)&ucVal, sizeof(ucVal));
}
bool CLuaBinary::getBool(void)
{
    return ((Q_INIT_NUMBER == readNumber<unsigned char>()) ? false : true);
}

bool CLuaBinary::setSint16(const short sVal)
{
    short sTmp = ntohs(sVal);

    return setVal((const char *)&sTmp, sizeof(sTmp));
}
short CLuaBinary::getSint16(void)
{
    return ntohs(readNumber<short>());
}
bool CLuaBinary::setUint16(const unsigned short usVal)
{
    unsigned short usTmp = ntohs(usVal);

    return setVal((const char *)&usTmp, sizeof(usTmp));
}
unsigned short CLuaBinary::getUint16(void)
{
    return ntohs(readNumber<unsigned short>());
}

bool CLuaBinary::setSint32(const int iVal)
{
    int iTmp = ntohl(iVal);

    return setVal((const char *)&iTmp, sizeof(iTmp));
}
int CLuaBinary::getSint32(void)
{
    return ntohl(readNumber<int>());
}
bool CLuaBinary::setUint32(const unsigned int uiVal)
{
    unsigned int uiTmp = ntohl(uiVal);

    return setVal((const char *)&uiTmp, sizeof(uiTmp));
}
unsigned int CLuaBinary::getUint32(void)
{
    return ntohl(readNumber<unsigned int>());
}

bool CLuaBinary::setSint64(const char *pszVal)
{
    int64_t iVal = Q_INIT_NUMBER;
    m_objStream.clear();
    m_objStream.str("");

    m_objStream << pszVal;
    m_objStream >> iVal;

    iVal = ntohl64(iVal);

    return setVal((const char *)&iVal, sizeof(iVal));
}
const char *CLuaBinary::getSint64(void)
{
    int64_t iVal = ntohl64(readNumber<int64_t>());

    m_strVali64.clear();
    m_objStream.clear();
    m_objStream.str("");

    m_objStream << iVal;

    m_objStream >> m_strVali64;

    return m_strVali64.c_str();
}
bool CLuaBinary::setUint64(const char *pszVal)
{
    uint64_t uiVal = Q_INIT_NUMBER;
    m_objStream.clear();
    m_objStream.str("");

    m_objStream << pszVal;
    m_objStream >> uiVal;

    uiVal = ntohl64(uiVal);

    return setVal((const char *)&uiVal, sizeof(uiVal));
}
const char *CLuaBinary::getUint64(void)
{
    uint64_t uiVal = ntohl64(readNumber<uint64_t>());

    m_strVali64.clear();
    m_objStream.clear();
    m_objStream.str("");

    m_objStream << uiVal;

    m_objStream >> m_strVali64;

    return m_strVali64.c_str();
}

bool CLuaBinary::setDouble(const double dVal)
{
    return setVal((const char *)&dVal, sizeof(dVal));
}
double CLuaBinary::getDouble(void)
{
    return readNumber<double>();
}

bool CLuaBinary::setFloat(const float fVal)
{
    return setVal((const char *)&fVal, sizeof(fVal));
}
float CLuaBinary::getFloat(void)
{
    return readNumber<float>();
}

bool CLuaBinary::setString(const char *pszVal)
{
    bool bOk = setVal(pszVal, strlen(pszVal));
    if (bOk)
    {
        bOk = skipWrite(1);
    }

    return bOk;
}
std::string CLuaBinary::getString(void)
{
    if (m_iCurParseLens >= m_iParseBufLens)
    {
        return std::string("");
    }

    std::string strVal(m_pParseBuffer + m_iCurParseLens);

    m_iCurParseLens += (strVal.size() + 1);

    return strVal;
}

bool CLuaBinary::setByte(const char *pszVal, const size_t iLens)
{
    return setVal(pszVal, iLens);
}

std::string CLuaBinary::getByte(const size_t iLens)
{
    if ((m_iCurParseLens + iLens) > m_iParseBufLens)
    {
        return std::string("");
    }

    std::string strVal(m_pParseBuffer + m_iCurParseLens, iLens);

    m_iCurParseLens += iLens;

    return strVal;
}

luabridge::LuaRef CLuaBinary::getStructAttr(luabridge::LuaRef &objAttr,
    std::string &strName, DataType &emType, int &iSize)
{
    strName.clear();
    emType = DType_SINT8;
    iSize = Q_INIT_NUMBER;
    luabridge::LuaRef objTable = luabridge::newTable(m_pLua);
    
    for (int j = 1; j <= objAttr.length(); j++)
    {
        switch(j)
        {
        case StAttr_Name:
            {
                strName = objAttr[j].cast<std::string>();
            }
            break;
        case StAttr_Type:
            {
                emType = (DataType)objAttr[j].cast<int>();
            }
            break;
        case StAttr_Size:
            {
                iSize = objAttr[j].cast<int>();
            }
            break;
        case StAttr_StAttr:
            {
                objTable = objAttr[j];
            }
            break;
        default:
            break;
        }
    }

    return objTable;
}

bool CLuaBinary::setStruct(luabridge::LuaRef objVal, luabridge::LuaRef objAttr)
{
    assert(objVal.isTable());
    assert(objAttr.isTable());

    bool bOk = false;
    DataType emType;
    int iSize = Q_INIT_NUMBER;
    std::string strName;

    for (int i = 1; i <= objAttr.length(); i++)
    {
        luabridge::LuaRef objTmp = objAttr[i];
        if (!objTmp.isTable()
            || STATTRTAB_MINLENS > objTmp.length())
        {
            return false;
        }

        luabridge::LuaRef objChildStAttr = getStructAttr(objTmp, strName, emType, iSize);
        luabridge::LuaRef objFieldVal = objVal[strName];

        switch(emType)
        {
        case DType_SINT8:
            {
                bOk = setSint8(getLuaNumber<short>(objFieldVal));
            }
            break;
        case DType_UINT8:
            {
                bOk = setUint8(getLuaNumber<unsigned short>(objFieldVal));
            }
            break;
        case DType_BOOL:
            {
                bOk = setBool(((Q_INIT_NUMBER != getLuaNumber<unsigned short>(objFieldVal)) ? true : false));
            }
            break;
        case DType_SINT16:
            {
                bOk = setSint16(getLuaNumber<short>(objFieldVal));
            }
            break;
        case DType_UINT16:
            {
                bOk = setUint16(getLuaNumber<unsigned short>(objFieldVal));
            }
            break;
        case DType_SINT32:
            {
                bOk = setSint32(getLuaNumber<int>(objFieldVal));
            }
            break;
        case DType_UINT32:
            {
                bOk = setUint32(getLuaNumber<unsigned int>(objFieldVal));
            }
            break;
        case DType_SINT64:
            {
                bOk = setSint64(Q_ToString(getLuaNumber<int64_t>(objFieldVal)).c_str());
            }
            break;
        case DType_UINT64:
            {
                bOk = setUint64(Q_ToString(getLuaNumber<uint64_t>(objFieldVal)).c_str());
            }
            break;
        case DType_FLOAT:
            {
                bOk = setFloat(getLuaNumber<float>(objFieldVal));
            }
            break;
        case DType_DOUBLE:
            {
                bOk = setDouble(getLuaNumber<double>(objFieldVal));
            }
            break;
        case DType_STRING:
            {
                if (!objFieldVal.isString())
                {
                    bOk = false;
                    break;
                }

                const char *pszVal = objFieldVal.cast<const char*>();
                if (objFieldVal.length() >= iSize)
                {
                    bOk = false;
                    Q_Printf("%s", "data len too large.");
                    break;
                }
               
                bOk = setString(pszVal);
                if (bOk)
                {
                    bOk = skipWrite(iSize - (objFieldVal.length() + 1));
                }
            }
            break;
        case DType_BYTE:
            {
                if (!objFieldVal.isString())
                {
                    bOk = false;
                    break;
                }

                const char *pszVal = objFieldVal.cast<const char*>();
                if (objFieldVal.length() > iSize)
                {
                    bOk = false;
                    Q_Printf("%s", "data len too large.");
                    break;
                }

                bOk = setByte(pszVal, objFieldVal.length());
                if (bOk)
                {
                    bOk = skipWrite(iSize - objFieldVal.length());
                }
            }
            break;
        case DType_STRUCT:
            {
                if (!objFieldVal.isTable()
                    || !objChildStAttr.isTable())
                {
                    bOk = false;
                    break;
                }

                bOk = setStruct(objFieldVal, objChildStAttr);
            }
            break;
        case DType_SKIP:
            {
                if (iSize > Q_INIT_NUMBER)
                {
                    bOk = skipWrite(iSize);
                }
            }
            break;

        default:
            bOk = false;
            break;
        }

        if (!bOk)
        {
            return bOk;
        }
    }

    return true;
}
luabridge::LuaRef CLuaBinary::getStruct(luabridge::LuaRef objAttr)
{
    assert(objAttr.isTable());

    DataType emType;
    int iSize = Q_INIT_NUMBER;
    std::string strName;
    luabridge::LuaRef objVal = luabridge::newTable(m_pLua);

    for (int i = 1; i <= objAttr.length(); i++)
    {
        luabridge::LuaRef objTmp = objAttr[i];
        if (!objTmp.isTable()
            || STATTRTAB_MINLENS > objTmp.length())
        {
            break;
        }

        luabridge::LuaRef objChildStAttr = getStructAttr(objTmp, strName, emType, iSize);
        switch(emType)
        {
        case DType_SINT8:
            {
                objVal[strName] = getSint8();
            }
            break;
        case DType_UINT8:
            {
                objVal[strName] = getUint8();
            }
            break;
        case DType_BOOL:
            {
                objVal[strName] = getBool();
            }
            break;
        case DType_SINT16:
            {
                objVal[strName] = getSint16();
            }
            break;
        case DType_UINT16:
            {
                objVal[strName] = getUint16();
            }
            break;
        case DType_SINT32:
            {
                objVal[strName] = getSint32();
            }
            break;
        case DType_UINT32:
            {
                objVal[strName] = getUint32();
            }
            break;
        case DType_SINT64:
            {
                objVal[strName] = getSint64();
            }
            break;
        case DType_UINT64:
            {
                objVal[strName] = getUint64();
            }
            break;
        case DType_FLOAT:
            {
                objVal[strName] = getFloat();
            }
            break;
        case DType_DOUBLE:
            {
                objVal[strName] = getDouble();
            }
            break;
        case DType_STRING:
            {
                std::string strVal = getString();
                objVal[strName] = strVal;
                skipRead(iSize - (strVal.size() + 1));
            }
            break;
        case DType_BYTE:
            {
                objVal[strName] = getByte(iSize);
            }
            break;
        case DType_STRUCT:
            {
                objVal[strName] = getStruct(objChildStAttr);
            }
            break;
        case DType_SKIP:
            {
                if (iSize > Q_INIT_NUMBER)
                {
                    skipRead(iSize);
                }
            }
            break;

        default:
            break;
        }
    }

    return objVal;
}

std::string CLuaBinary::getBuffer(void) const
{
    return std::string(m_objWritBuffer.getBuffer(), m_objWritBuffer.getLens());
}
