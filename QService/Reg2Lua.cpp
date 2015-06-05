
#include "Reg2Lua.h"

void Q_LuaLog(int iLV, const char *pszMsg)
{
    Q_LOG((LOG_LEVEL)iLV, "%s", pszMsg);
}

void Q_DBLog(const char *pszPlayerID, unsigned short sType, const char *pszMsg, const unsigned int iLens)
{
    Q_DBLOG(pszPlayerID, sType, pszMsg, iLens);
}

void sendMail(std::string strMail)
{
    CMailSender::getSingletonPtr()->sendMail(strMail);
}

const char *Q_GetModulPath(void)
{
    return g_acModulPath;
}

const char *Q_GetPathSeparator(void)
{
    return Q_PATH_SEPARATOR;
}

int getServerID(void)
{
    return g_iServerID;
}

const char *getServerNam(void)
{
    return g_strServerName.c_str();
}

CReg2Lua::CReg2Lua(void) : m_pstLState(NULL)
{

}

CReg2Lua::~CReg2Lua(void)
{
    m_pstLState = NULL;
}

void CReg2Lua::Register(void)
{
    reg_Func();
    reg_Timer();
    reg_SnowflakeID();
    reg_Filter();
    reg_Charset();
    reg_Ini();
    reg_TableFile();
    reg_Session();
    reg_SessionManager();
    reg_HttpClient();
    reg_HttpBuffer();
    reg_Binary();
    reg_Encrypt();
}

void CReg2Lua::reg_Func(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .addFunction("Q_GetModulPath", Q_GetModulPath)
        .addFunction("Q_GetPathSeparator", Q_GetPathSeparator)
        .addFunction("Q_LOG", Q_LuaLog)
        .addFunction("Q_DBLOG", Q_DBLog)
        .addFunction("sendMail", sendMail)
        .addFunction("getServerID", getServerID)
        .addFunction("getServerNam", getServerNam);
}

void CReg2Lua::reg_Timer(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CTimer>("CTimer")
            .addConstructor<void (*) (void)>()

            .addFunction("reStart", &CTimer::reStart)
            .addFunction("Elapsed", &CTimer::Elapsed)
        .endClass();
}

void CReg2Lua::reg_SnowflakeID(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CSnowflakeID>("CSnowflakeID")
            .addConstructor<void (*) (void)>()

            .addFunction("setMachineID", &CSnowflakeID::setMachineID)
            .addFunction("getSnowflakeID", &CSnowflakeID::getSnowflakeID)
        .endClass();
}

void CReg2Lua::reg_Filter(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CFilter>("CFilter")
            .addConstructor<void (*) (void)>()

            .addFunction("Clear", &CFilter::Clear)

            .addFunction("addFilterWord", &CFilter::addFilterWord)

            .addFunction("checkHave", &CFilter::checkHave)
            .addFunction("Filter", &CFilter::Filter)
        .endClass();
}

void CReg2Lua::reg_Charset(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CCharset>("CCharset")
            .addConstructor<void (*) (void)>()

            .addFunction("getStrCharset", &CCharset::getStrCharset)
        .endClass();
}

void CReg2Lua::reg_Ini(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CIniFile>("CIniFile")
            .addConstructor<void (*) (void)>()

            .addFunction("setFile", &CIniFile::setFile)

            .addFunction("getStringValue", &CIniFile::getStringValue)
            .addFunction("getIntValue", &CIniFile::getIntValue)
            .addFunction("getFloatValue", &CIniFile::getFloatValue)
        .endClass();
}

void CReg2Lua::reg_TableFile(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CTableFile>("CTableFile")
            .addConstructor<void (*) (void)>()

            .addFunction("setFile", &CTableFile::setFile)
            .addFunction("setSplitFlag", &CTableFile::setSplitFlag)

            .addFunction("Parse", &CTableFile::Parse)
            .addFunction("eof", &CTableFile::eof)
            .addFunction("nextRow", &CTableFile::nextRow)
            .addFunction("reSet", &CTableFile::reSet)

            .addFunction("getStringValue", &CTableFile::getStringValue)
            .addFunction("getIntValue", &CTableFile::getIntValue)
            .addFunction("getFloatValue", &CTableFile::getFloatValue)
        .endClass();
}

void CReg2Lua::reg_Session(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CSession>("CSession")
            .addFunction("getSessionID", &CSession::getSessionID)

            .addFunction("setExterID", &CSession::setExterID)
            .addFunction("getExterID", &CSession::getExterID)

            .addFunction("setIDCard", &CSession::setIDCard)
            .addFunction("getIDCard", &CSession::getIDCard)

            .addFunction("setPing", &CSession::setPing)
            .addFunction("getPing", &CSession::getPing)

            .addFunction("setAccount", &CSession::setAccount)
            .addFunction("getAccount", &CSession::getAccount)

            .addFunction("setStatus", &CSession::setStatus)
            .addFunction("getStatus", &CSession::getStatus)

            .addFunction("getType", &CSession::getType)
        .endClass();
}

void CReg2Lua::reg_SessionManager(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CSessionManager>("CSessionManager")
            .addFunction("closeLinkByID", &CSessionManager::closeLinkByID)
            .addFunction("closeCurLink", &CSessionManager::closeCurLink)

            .addFunction("getCurSession", &CSessionManager::getCurSession)
            .addFunction("getSessionByID", &CSessionManager::getSessionByID)

            .addFunction("sendToCur", &CSessionManager::sendToCur)
            .addFunction("sendToByID", &CSessionManager::sendToByID)

            .addFunction("sendBToCur", &CSessionManager::sendBToCur)
            .addFunction("sendBToByID", &CSessionManager::sendBToByID)

            .addFunction("getTimer", &CSessionManager::getTimer)
            .addFunction("getCount", &CSessionManager::getCount)

            .addFunction("getSessionSize", &CSessionManager::getSessionSize)

            .addFunction("confirmStop", &CSessionManager::confirmStop)

            .addFunction("addLinkOther", &CSessionManager::addLinkOther)
            .addFunction("getLinkOtherID", &CSessionManager::getLinkOtherID)
            .addFunction("getLinkOtherByType", &CSessionManager::getLinkOtherByType)
        .endClass();
}

void CReg2Lua::reg_HttpClient(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CHttpClient>("CHttpClient")
            .addConstructor<void (*) (void)>()

            .addFunction("Post", &CHttpClient::Post)
            .addFunction("Get", &CHttpClient::Get)
            .addFunction("Posts", &CHttpClient::Posts)
            .addFunction("Gets", &CHttpClient::Gets)
        .endClass();
}

void CReg2Lua::reg_HttpBuffer(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CHttpParser>("CHttpBuffer")
            .addFunction("getQuery", &CHttpParser::getQuery)
            .addFunction("getPostMsg", &CHttpParser::getPostMsg)
            .addFunction("setReplyContent", &CHttpParser::setReplyContent)
            .addFunction("Reply", &CHttpParser::Reply)
        .endClass();
}

void CReg2Lua::reg_Binary(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CLuaBinary>("CLuaBinary")
            .addFunction("setBuffer", &CLuaBinary::setBuffer)   

            .addFunction("getLens", &CLuaBinary::getLens)

            .addFunction("reSetWrite", &CLuaBinary::reSetWrite)
            .addFunction("skipRead", &CLuaBinary::skipRead)
            .addFunction("skipWrite", &CLuaBinary::skipWrite)

            .addFunction("setSint8", &CLuaBinary::setSint8)
            .addFunction("getSint8", &CLuaBinary::getSint8)
            .addFunction("setUint8", &CLuaBinary::setUint8)
            .addFunction("getUint8", &CLuaBinary::getUint8)

            .addFunction("setBool", &CLuaBinary::setBool)
            .addFunction("getBool", &CLuaBinary::getBool)

            .addFunction("setSint16", &CLuaBinary::setSint16)
            .addFunction("getSint16", &CLuaBinary::getSint16)
            .addFunction("setUint16", &CLuaBinary::setUint16)
            .addFunction("getUint16", &CLuaBinary::getUint16)

            .addFunction("setSint32", &CLuaBinary::setSint32)
            .addFunction("getSint32", &CLuaBinary::getSint32)
            .addFunction("setUint32", &CLuaBinary::setUint32)
            .addFunction("getUint32", &CLuaBinary::getUint32)

            .addFunction("setSint64", &CLuaBinary::setSint64)
            .addFunction("getSint64", &CLuaBinary::getSint64)
            .addFunction("setUint64", &CLuaBinary::setUint64)
            .addFunction("getUint64", &CLuaBinary::getUint64)

            .addFunction("setDouble", &CLuaBinary::setDouble)
            .addFunction("getDouble", &CLuaBinary::getDouble)

            .addFunction("setFloat", &CLuaBinary::setFloat)
            .addFunction("getFloat", &CLuaBinary::getFloat)

            .addFunction("setString", &CLuaBinary::setString)
            .addFunction("getString", &CLuaBinary::getString)

            .addFunction("setByte", &CLuaBinary::setByte)
            .addFunction("getByte", &CLuaBinary::getByte)

            .addFunction("setStruct", &CLuaBinary::setStruct)
            .addFunction("getStruct", &CLuaBinary::getStruct)

            .addFunction("getBuffer", &CLuaBinary::getBuffer)
        .endClass();
}

void CReg2Lua::reg_Encrypt(void)
{
    luabridge::getGlobalNamespace(m_pstLState)
        .beginClass<CEncrypt>("CEncrypt")
            .addFunction("b64Encode", &CEncrypt::b64Encode)
            .addFunction("b64Decode", &CEncrypt::b64Decode)

            .addFunction("md5Str", &CEncrypt::md5Str)
            .addFunction("md5File", &CEncrypt::md5File)

            .addFunction("sha1Str", &CEncrypt::sha1Str)
            .addFunction("sha1File", &CEncrypt::sha1File)

            .addFunction("aesEncode", &CEncrypt::aesEncode)
            .addFunction("aesDecode", &CEncrypt::aesDecode)

            .addFunction("rsaPubEncode", &CEncrypt::rsaPubEncode)
            .addFunction("rsaPriDecode", &CEncrypt::rsaPriDecode)
            .addFunction("rsaPriEncode", &CEncrypt::rsaPriEncode)
            .addFunction("rsaPubDecode", &CEncrypt::rsaPubDecode)

            .addFunction("urlEncode", &CEncrypt::urlEncode)
            .addFunction("urlDecode", &CEncrypt::urlDecode)

            .addFunction("zlibEncode", &CEncrypt::zlibEncode)
            .addFunction("zlibDecode", &CEncrypt::zlibDecode)
        .endClass();
}
