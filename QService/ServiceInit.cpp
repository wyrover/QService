
#include "ServiceInit.h"

/*业务处理头文件*/

enum 
{
    INICOFG_CLOSE = 0,
    INICOFG_OPEN
};

#define CONFIG_SERVER      "config_server.xml"
#define CONFIG_TXTLOG      "config_txtlog.xml"
#define CONFIG_DBLOG       "config_dblog.xml"
#define CONFIG_LINKOTHER   "config_linkother.xml"
#define CONFIG_MAIL        "config_mail.xml"
#define CONFIG_AES         "config_aes.xml"
#define CONFIG_RSA         "config_rsa.xml"

#define CONFIG_FOLDER      "config"
#define LOG_FOLDER         "log"

#define RSAKEY_FOLDER      "rsakeys"

struct LinkOther
{
    unsigned short usPort;
    unsigned short usType;
    std::string strIp;
    std::string strName;
};

int initAESKey(void)
{
    xml_node objXmlNode;
    xml_document objXmlDoc;
    xml_parse_result objXmlResult;

    std::string strConfig = Q_FormatStr("%s%s%s%s", 
        g_acModulPath, CONFIG_FOLDER, Q_PATH_SEPARATOR, CONFIG_AES);

    objXmlResult = objXmlDoc.load_file(strConfig.c_str());
    if (status_ok != objXmlResult.status)
    {
        Q_Printf("%s", "load aes config error.");

        return Q_RTN_FAILE;
    }

    objXmlNode = objXmlDoc.child("QServer").child("AES");
    if (objXmlNode.empty())
    {
        return Q_RTN_OK;
    }

    std::string strKey = objXmlNode.child_value("Key");
    unsigned short usType = (unsigned short)atoi(objXmlNode.child_value("KeyType"));

    CEncrypt::getSingletonPtr()->setAESKey(strKey.c_str(), usType);

    return Q_RTN_OK;
}

int initRSAKey(void)
{
    xml_node objXmlNode;
    xml_document objXmlDoc;
    xml_parse_result objXmlResult;

    std::string strConfig = Q_FormatStr("%s%s%s%s", 
        g_acModulPath, CONFIG_FOLDER, Q_PATH_SEPARATOR, CONFIG_RSA);

    objXmlResult = objXmlDoc.load_file(strConfig.c_str());
    if (status_ok != objXmlResult.status)
    {
        Q_Printf("%s", "load rsa config error.");

        return Q_RTN_FAILE;
    }

    objXmlNode = objXmlDoc.child("QServer").child("RSA");
    if (objXmlNode.empty())
    {
        return Q_RTN_OK;
    }

    std::string strPub = Q_FormatStr("%s%s%s%s", 
        g_acModulPath, RSAKEY_FOLDER, Q_PATH_SEPARATOR, objXmlNode.child_value("PubKey"));
    std::string strPri = Q_FormatStr("%s%s%s%s", 
        g_acModulPath, RSAKEY_FOLDER, Q_PATH_SEPARATOR, objXmlNode.child_value("PriKey"));
    std::string strRand = Q_FormatStr("%s%s%s%s", 
        g_acModulPath, RSAKEY_FOLDER, Q_PATH_SEPARATOR, objXmlNode.child_value("RandKey"));

    if ((Q_RTN_OK != Q_FileExist(strPub.c_str()))
        || (Q_RTN_OK != Q_FileExist(strPri.c_str()))
        || (Q_RTN_OK != Q_FileExist(strRand.c_str())))
    {
        CRSAKey objKey;

        if (Q_RTN_OK != objKey.creatKey(512))
        {
            Q_Printf("%s", "create rsa key error.");

            return Q_RTN_FAILE;
        }

        if ((Q_RTN_OK != objKey.savePublicKey(strPub.c_str())
            || (Q_RTN_OK != objKey.savePrivateKey(strPri.c_str()))
            || (Q_RTN_OK != objKey.saveRandom(strRand.c_str()))))
        {
            Q_Printf("%s", "save rsa key error.");

            return Q_RTN_FAILE;
        }
    }

    if (Q_RTN_OK != CEncrypt::getSingletonPtr()->setRSAKey(strPub.c_str(), strPri.c_str(), strRand.c_str()))
    {
        Q_Printf("%s", "load rsa key error.");

        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

int initEncrypt(void)
{
    if (Q_RTN_OK != initAESKey())
    {
        return Q_RTN_FAILE;
    }    

    return initRSAKey();
}

int rendMailConfig(std::string &strMailServer,
    std::string &strSender,
    std::string &strUserName,
    std::string &strPassWord,
    enum jwsmtp::mailer::authtype &emType,
    bool &bOpen)
{
    xml_node objXmlNode;
    xml_document objXmlDoc;
    xml_parse_result objXmlResult;

    bOpen = true;
    std::string strConfig = Q_FormatStr("%s%s%s%s", 
        g_acModulPath, CONFIG_FOLDER, Q_PATH_SEPARATOR, CONFIG_MAIL);

    objXmlResult = objXmlDoc.load_file(strConfig.c_str());
    if (status_ok != objXmlResult.status)
    {
        Q_Printf("%s", "load mail config error.");

        return Q_RTN_FAILE;
    }

    objXmlNode = objXmlDoc.child("QServer").child("Mail");
    if (objXmlNode.empty())
    {
        bOpen = false;

        return Q_RTN_OK;
    }

    strMailServer = objXmlNode.child_value("MailServer");
    emType = (enum jwsmtp::mailer::authtype)atoi(objXmlNode.child_value("AuthType"));
    strSender = objXmlNode.child_value("Sender");
    strUserName = objXmlNode.child_value("UserName");
    strPassWord = objXmlNode.child_value("PassWord");
    if (strMailServer.empty())
    {
        Q_Printf("%s", "mail server is empty.");
        return Q_RTN_FAILE;
    }
    if (strSender.empty())
    {
        Q_Printf("%s", "mail sender is empty.");
        return Q_RTN_FAILE;
    }
    if (strUserName.empty())
    {
        Q_Printf("%s", "mail username is empty.");
        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

int initMailSender(void)
{
    std::string strMailServer;
    std::string strSender;
    std::string strUserName;
    std::string strPassWord;
    enum jwsmtp::mailer::authtype emType;
    bool bOpen;

    if (Q_RTN_OK != rendMailConfig(strMailServer, strSender, strUserName, strPassWord, emType, bOpen))
    {
        return Q_RTN_FAILE;
    }

    if (!bOpen)
    {
        return Q_RTN_OK;
    }

    CMailSender::getSingletonPtr()->setServer(strMailServer);
    CMailSender::getSingletonPtr()->setAuthType(emType);
    CMailSender::getSingletonPtr()->setSender(strSender);
    CMailSender::getSingletonPtr()->setUserName(strUserName);
    CMailSender::getSingletonPtr()->setPassWord(strPassWord);

    CThread objThread;
    objThread.Execute(CMailSender::getSingletonPtr(), false);
    if (!CMailSender::getSingletonPtr()->waitForStarted())
    {
        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

void stopMailSender(void)
{
    CMailSender::getSingletonPtr()->Stop();
}

int initTxtLoger(void)
{
    int iMaxSize = 5*1024*1024;
    int iPriority = LOGLV_DEBUG;
    std::string strLogName;
    xml_node objXmlNode;
    xml_document objXmlDoc;
    xml_parse_result objXmlResult;

    std::string strConfig = Q_FormatStr("%s%s%s%s", 
        g_acModulPath, CONFIG_FOLDER, Q_PATH_SEPARATOR, CONFIG_TXTLOG);
    std::string strLogPath = Q_FormatStr("%s%s%s", g_acModulPath, LOG_FOLDER, Q_PATH_SEPARATOR);

    objXmlResult = objXmlDoc.load_file(strConfig.c_str());
    if (status_ok != objXmlResult.status)
    {
        Q_Printf("%s", "load txtlog config error.");

        return Q_RTN_FAILE;
    }

    objXmlNode = objXmlDoc.child("QServer").child("TxtLog");
    if (!objXmlNode.empty())
    {
        strLogName = objXmlNode.child_value("LogName");
        Q_Printf("log file %s.", strLogName.c_str());

        iMaxSize = atoi(objXmlNode.child_value("MaxSize"));
        Q_Printf("log file size %d.", iMaxSize);

        iPriority = atoi(objXmlNode.child_value("Priority"));
        Q_Printf("log priority %d.", iPriority);
    }

    CTxtLoger::getSingletonPtr()->setLogMaxSize(iMaxSize);    
    CTxtLoger::getSingletonPtr()->setPriority((LOG_LEVEL)iPriority);
    CTxtLoger::getSingletonPtr()->setLogFile(std::string(strLogPath + strLogName).c_str());
    CTxtLoger::getSingletonPtr()->Open();

    return Q_RTN_OK;
}

int initDBLoger(bool &bUseDBLog)
{
    std::string strIp;
    std::string strUser;
    std::string strPWD;
    std::string strDB;
    unsigned short usPort = Q_INIT_NUMBER;
    xml_node objXmlNode;
    xml_document objXmlDoc;
    xml_parse_result objXmlResult;

    std::string strConfig = Q_FormatStr("%s%s%s%s", 
        g_acModulPath, CONFIG_FOLDER, Q_PATH_SEPARATOR, CONFIG_DBLOG);

    objXmlResult = objXmlDoc.load_file(strConfig.c_str());
    if (status_ok != objXmlResult.status)
    {
        Q_Printf("%s", "load dblog config error.");

        return Q_RTN_FAILE;
    }

    objXmlNode = objXmlDoc.child("QServer").child("DBLog");
    if (objXmlNode.empty())
    {
        bUseDBLog = false;

        return Q_RTN_OK;
    }

    bUseDBLog = true;
    strIp = objXmlNode.child_value("Ip");
    strUser = objXmlNode.child_value("User");
    strPWD = objXmlNode.child_value("PWD");
    strDB = objXmlNode.child_value("DB");
    usPort = (unsigned short)atoi(objXmlNode.child_value("Port"));
    Q_Printf("link to DB.ip %s port %d database %s...", 
        strIp.c_str(), usPort, strDB.c_str());
    if (!CDBLoger::getSingletonPtr()->Init(strIp.c_str(), usPort, strUser.c_str(), strPWD.c_str(), strDB.c_str()))
    {
        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

int initLogSystem(void)
{
    int iRtn = Q_RTN_OK;
    bool bUseDBLog = false;
    CThread objThread;

    Q_Printf("%s", "start log system...");
    objThread.Execute(CLog::getSingletonPtr(), false);
    if (!CLog::getSingletonPtr()->waitForStarted())
    {
        Q_Printf("%s", "start log system error.");

        return Q_RTN_FAILE;
    }

    Q_Printf("%s", "init txt loger ...");
    iRtn = initTxtLoger();
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "init txt loger error.");

        return iRtn;
    }

    Q_Printf("%s", "init db loger ...");
    iRtn = initDBLoger(bUseDBLog);
    if (Q_RTN_OK != iRtn)
    {
        Q_Printf("%s", "init db loger error.");

        return iRtn;
    }

    CLog::getSingletonPtr()->addLoger(CTxtLoger::getSingletonPtr());
    if (bUseDBLog)
    {
        CLog::getSingletonPtr()->addLoger(CDBLoger::getSingletonPtr());
    }

    return Q_RTN_OK;
}

void stopLogSystem(void)
{
    CLog::getSingletonPtr()->Stop();
}

int readLinkConfig(std::vector<LinkOther> &vcLinkOther)
{
    xml_node objXmlNode;
    xml_document objXmlDoc;
    xml_parse_result objXmlResult;
    xpath_node_set objNodeSet;
    xpath_node_set::const_iterator itNode;

    std::string strConfig = Q_FormatStr("%s%s%s%s", 
        g_acModulPath, CONFIG_FOLDER, Q_PATH_SEPARATOR, CONFIG_LINKOTHER);

    objXmlResult = objXmlDoc.load_file(strConfig.c_str());
    if (status_ok != objXmlResult.status)
    {
        Q_Printf("%s", "load link other config error.");

        return Q_RTN_FAILE;
    }

    objNodeSet = objXmlDoc.select_nodes("//LinkOther");
    for (itNode = objNodeSet.begin(); objNodeSet.end() != itNode; itNode++)
    {
        LinkOther stLinkOther;
        stLinkOther.strIp = itNode->node().child_value("IP");
        stLinkOther.strName = itNode->node().child_value("Name");
        stLinkOther.usPort = (unsigned short)atoi(itNode->node().child_value("Port"));
        stLinkOther.usType = (unsigned short)atoi(itNode->node().child_value("Type"));

        vcLinkOther.push_back(stLinkOther);
    }

    return Q_RTN_OK;
}

int initServerLinker(void)
{
    std::vector<LinkOther> vcLinkOther;
    std::vector<LinkOther>::iterator itHost;

    if (Q_RTN_OK != readLinkConfig(vcLinkOther))
    {
        return Q_RTN_FAILE;
    }

    for (itHost = vcLinkOther.begin(); vcLinkOther.end() != itHost; itHost++)
    {
        if (!CLinkOther::getSingletonPtr()->addHost(itHost->strIp.c_str(), 
                itHost->usPort, itHost->strName.c_str(), itHost->usType))
        {
            return Q_RTN_FAILE;
        }
    }

    if (Q_RTN_OK != CLinkOther::getSingletonPtr()->Start())
    {
        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

void stopServerLinker(void)
{
    CLinkOther::getSingletonPtr()->Stop();
}

int readConfig(unsigned int &uiMS, std::string &strLua,
    std::map<unsigned short, std::string> &mapDebug, 
    std::map<unsigned short, std::string> &mapTcp, 
    std::map<unsigned short, std::string> &mapWebSock, 
    std::map<unsigned short, std::string> &mapHttp)
{
    std::string strIp;
    unsigned short usPort = Q_INIT_NUMBER;
    xpath_node_set objNodeSet;
    xpath_node_set::const_iterator itNode;
    xml_node objXmlNode;
    xml_document objXmlDoc;
    xml_parse_result objXmlResult;

    std::string strConfig = Q_FormatStr("%s%s%s%s", 
        g_acModulPath, CONFIG_FOLDER, Q_PATH_SEPARATOR, CONFIG_SERVER);

    objXmlResult = objXmlDoc.load_file(strConfig.c_str());
    if (status_ok != objXmlResult.status)
    {
        Q_Printf("%s", "load service config error.");

        return Q_RTN_FAILE;
    }

    objNodeSet = objXmlDoc.select_nodes("//ServerInfo");
    if (objNodeSet.empty())
    {
        Q_LOG(LOGLV_ERROR, "%s", "no find node //ServerInfo.");

        return Q_RTN_FAILE;
    }

    itNode = objNodeSet.begin();

    g_iServerID =  atoi(itNode->node().child_value("ServerID"));
    g_strServerName = itNode->node().child_value("Name");
    Q_Printf("service id %d, name %s.", g_iServerID, g_strServerName.c_str());

    strLua = std::string(g_acModulPath) + 
        std::string(itNode->node().child_value("Script"));
    uiMS = (unsigned int)atoi(itNode->node().child_value("Timer"));

    objNodeSet = objXmlDoc.select_nodes("//Debug");
    if (!objNodeSet.empty())
    {
        for (itNode = objNodeSet.begin(); objNodeSet.end() != itNode; itNode++)
        {
            strIp = itNode->node().child_value("BindIP");
            usPort = (unsigned short)atoi(itNode->node().child_value("Port"));
        }

        (void)mapDebug.insert(std::make_pair(usPort, strIp));
    }

    objNodeSet = objXmlDoc.select_nodes("//TCP");
    if (!objNodeSet.empty())
    {
        for (itNode = objNodeSet.begin(); objNodeSet.end() != itNode; itNode++)
        {
            strIp = itNode->node().child_value("BindIP");
            usPort = (unsigned short)atoi(itNode->node().child_value("Port"));
        }

        (void)mapTcp.insert(std::make_pair(usPort, strIp));
    }

    objNodeSet = objXmlDoc.select_nodes("//WebSock");
    if (!objNodeSet.empty())
    {
        for (itNode = objNodeSet.begin(); objNodeSet.end() != itNode; itNode++)
        {
            strIp = itNode->node().child_value("BindIP");
            usPort = (unsigned short)atoi(itNode->node().child_value("Port"));
        }

        (void)mapWebSock.insert(std::make_pair(usPort, strIp));
    }

    objNodeSet = objXmlDoc.select_nodes("//Http");
    if (!objNodeSet.empty())
    {
        for (itNode = objNodeSet.begin(); objNodeSet.end() != itNode; itNode++)
        {
            strIp = itNode->node().child_value("BindIP");
            usPort = (unsigned short)atoi(itNode->node().child_value("Port"));
        }

        (void)mapHttp.insert(std::make_pair(usPort, strIp));
    }

    return Q_RTN_OK;
}

int initServer(void)
{
    int iRtn = Q_RTN_OK;
    unsigned int uiMS = Q_INIT_NUMBER;
    std::string strLua;
    std::map<unsigned short, std::string> mapDebug;
    std::map<unsigned short, std::string> mapTcp;
    std::map<unsigned short, std::string> mapWebSock;
    std::map<unsigned short, std::string> mapHttp;

    iRtn = readConfig(uiMS, strLua, mapDebug, mapTcp, mapWebSock, mapHttp);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    Q_Printf("%s", "init lua vm...");
    iRtn = CDisposeEvent::getSingletonPtr()->initLua(strLua);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    iRtn = CServer::getSingletonPtr()->Init(uiMS, CDisposeEvent::getSingletonPtr(), 
        CDisposeEvent::getSingletonPtr()->getLua(),
        mapDebug, mapTcp, mapWebSock, mapHttp);
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    CThread objThread;
    objThread.Execute(CServer::getSingletonPtr(), false);
    if (!CServer::getSingletonPtr()->waitForStarted())
    {
        Q_Printf("%s", "start service error.");

        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

void stopServer(void)
{
    CServer::getSingletonPtr()->Stop();
}


/*************************************************
* Function name:Service_InitProgram(void)
* Description  :初始化所有的服务
* IN           :NONE
* OUT          :NONE
* Return       :RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
**************************************************/
int Service_InitProgram(void)
{
    int iRtn = Q_RTN_OK;

    try
    {
        do 
        {
            Q_Printf("%s", "init log system...");
            iRtn = initLogSystem();
            if (Q_RTN_OK != iRtn)
            {
                break;
            }

            Q_Printf("%s", "init encrypt system...");
            iRtn = initEncrypt();
            if (Q_RTN_OK != iRtn)
            {
                break;
            }

            Q_Printf("%s", "init mail system...");
            iRtn = initMailSender();
            if (Q_RTN_OK != iRtn)
            {
                break;
            }

            Q_Printf("%s", "init service...");
            iRtn = initServer();
            if (Q_RTN_OK != iRtn)
            {
                break;
            }

            Q_Printf("%s", "init link other...");
            iRtn = initServerLinker();
            if (Q_RTN_OK != iRtn)
            {
                break;
            }

        } while (false);        
    }
    catch(CQException &e)
    {
        Q_Printf("get an exception. code %d, message %s", e.getErrorCode(), e.getErrorMsg());

        iRtn = e.getErrorCode();
    }

    if (Q_RTN_OK != iRtn)
    {
        Service_Exit();
    }
    else
    {
        Q_Printf("%s", "start service successfully.");
        Q_LOG(LOGLV_INFO, "%s", "start service successfully.");
    }

    return iRtn;
}

/*************************************************
* Function name:Service_Exit(void)
* Description  :退出
* IN           :NONE
* OUT          :NONE
* Return       :RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
**************************************************/
int Service_Exit(void)
{
    Q_Printf("%s", "stop service...");
    stopServer();
    Q_Printf("%s", "stop link other...");
    stopServerLinker();
    Q_Printf("%s", "stop mail system...");
    stopMailSender();
    Q_Printf("%s", "stop log system...");
    stopLogSystem();
    Q_Printf("%s", "stop service successfully.");

    return Q_RTN_OK;
}

/*************************************************
* Function name:SigHandEntry(int iSigNum)
* Description  :退出信号
* IN           :iSigNum --信号
* OUT          :NONE
* Return       :RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2011/06/07
* Modification 
* ......record :first program
**************************************************/
void SigHandEntry(int iSigNum)
{
#ifndef Q_OS_WIN
    Q_LOG(LOGLV_INFO, "catch signal %d.", iSigNum);
    Q_Printf("catch signal %d.", iSigNum);

    g_objExitMutex.Lock();
    g_objExitCond.Signal();
    Q_LOG(LOGLV_INFO, "%s", "signal stop service.");
    g_objExitMutex.unLock();
#endif
}
