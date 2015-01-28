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

#include "ServiceInit.h"

/*业务处理头文件*/

#define Q_CONFIGFILE      "config.xml"
#define Q_CONFIGFOLDER    "config"
#define LOG_FOLDER        "log"

CServerInit m_objServerInit;

/*日志任务*/
class CLogSysTask : public CTask
{
public:
    CLogSysTask(void) : m_pLog(NULL)
    {};
    ~CLogSysTask(void)
    {};

    void setLogSys(CLog *pLog)
    {
        m_pLog = pLog;
    };

    void Run(void)
    {
        m_pLog->Start();
    };

private:
    CLog *m_pLog;
};

/*****************************************************************************/

class CServerTask : public CTask
{
public:
    CServerTask(void) : m_pSV(NULL)
    {};
    ~CServerTask(void)
    {};

    void setServerHandle(CServer *pSV)
    {
        m_pSV = pSV;
    };

    void Run(void)
    {
        m_pSV->Start();
    };

private:
    CServer *m_pSV;
};

/*****************************************************************************/

CServerInit::~CServerInit(void)
{
    Stop();
}

int CServerInit::Start(void)
{
    int iRtn = Q_RTN_OK;
    CLogSysTask *pLogSysTask = NULL;

    m_strConfig = Q_FormatStr("%s%s%s%s", 
        g_acModulPath, Q_CONFIGFOLDER, Q_PATH_SEPARATOR, Q_CONFIGFILE);
    Q_Printf("config file %s.", m_strConfig.c_str()); 

    /*加载配置文件*/
    Q_Printf("%s", "load config file ...");
    m_objXmlResult = m_objXmlDoc.load_file(m_strConfig.c_str());
    if (status_ok != m_objXmlResult.status)
    {
        Q_Printf("%s", "load config file error.");

        return Q_RTN_FAILE;
    }

    /*初始化日志*/
    Q_Printf("%s", "init log system ...");
    pLogSysTask = new(std::nothrow) CLogSysTask();
    if (NULL == pLogSysTask)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

        return Q_ERROR_ALLOCMEMORY;
    }

    pLogSysTask->setLogSys(&m_objLog);
    m_objThread.Execute(pLogSysTask);
    if (!m_objLog.waitForStarted())
    {
        Q_Printf("%s", "log system start timeout.");

        return Q_RTN_FAILE;
    }

    initSampleLog();

    /*启动服务*/
    Q_Printf("%s", "start service ...");
    iRtn = initServer();
    if (Q_RTN_OK != iRtn)
    {
        return iRtn;
    }

    Q_Printf("%s", "start service successfully!");
    Q_LOG(LOGLV_INFO, "%s", "start service successful!");

    return Q_RTN_OK;
}

void CServerInit::Stop(void)
{
    destroyServer();
}

bool CServerInit::readConfig(void)
{
    pugi::xpath_node_set objTmpNodeSet;
    pugi::xpath_node_set::const_iterator itTmpNode;
    pugi::xpath_node_set objNodeSet;
    pugi::xpath_node_set::const_iterator itNode;

    objNodeSet = m_objXmlDoc.select_nodes("//ServerInfo");
    if (objNodeSet.empty())
    {
        Q_LOG(LOGLV_ERROR, "%s", "no find node //ServerInfo.");

        return false;
    }

    itNode = objNodeSet.begin();

    g_iServerID =  atoi(itNode->node().child_value("ServerID"));
    g_iServerType = atoi(itNode->node().child_value("Type"));
    Q_Printf("service id %d, type %d.", g_iServerID, g_iServerType);
    m_stServerConfig.strBindIP = itNode->node().child_value("BindIP");
    m_stServerConfig.usThreadNum = atoi(itNode->node().child_value("ThreadNum"));
    m_stServerConfig.strScript = std::string(g_acModulPath) + std::string(itNode->node().child_value("Script"));
    m_stServerConfig.uiTimer = atoi(itNode->node().child_value("Timer"));
    m_stServerConfig.usPort = atoi(itNode->node().child_value("Port"));
    Q_Printf("bind port %d.", m_stServerConfig.usPort);

    //ServerLinker
    objTmpNodeSet = itNode->node().select_nodes("ServerLinker");        
    for (itTmpNode = objTmpNodeSet.begin(); objTmpNodeSet.end() != itTmpNode; ++itTmpNode)
    {
        ServerLinkerInfo stLinkerInfo;

        stLinkerInfo.iType = atoi(itTmpNode->node().child_value("Type"));
        stLinkerInfo.strIP = itTmpNode->node().child_value("IP");
        stLinkerInfo.usPort = atoi(itTmpNode->node().child_value("Port"));
        stLinkerInfo.strLinkerName = itTmpNode->node().child_value("LinkerName");

        m_stServerConfig.lstLinkerInfo.push_back(stLinkerInfo);
    }

    return true;
}

int CServerInit::initServer(void)
{
    int iRtn = Q_RTN_OK;

    Q_Printf("%s", "read service config...");
    if (!readConfig())
    {
        Q_Printf("%s", "read service config error.");

        return Q_RTN_FAILE;
    }

    try
    {
        for (unsigned short usI = 0; usI < m_stServerConfig.usThreadNum; usI++)
        {
            CEventInterface *pInterface  = new(std::nothrow) CDisposeEvent(m_stServerConfig.strScript.c_str());
            if (NULL == pInterface)
            {
                Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

                return Q_ERROR_ALLOCMEMORY;
            }

            m_vcInterface.push_back(pInterface);
        }
    }
    catch(CException &e)
    {
        Q_Printf("get an exception. code %d, message %s", e.getErrorCode(), e.getErrorMsg());
        
        return e.getErrorCode();
    }    

    /*new CServer*/
    m_stServer.pSV = new(std::nothrow) CServer();
    if (NULL == m_stServer.pSV)
    {
        Q_LOG(LOGLV_ERROR, "%s", Q_EXCEPTION_ALLOCMEMORY);

        return Q_ERROR_ALLOCMEMORY;
    }

    /*设置值*/
    m_stServer.pSV->setBindIP(m_stServerConfig.strBindIP.c_str());
    m_stServer.pSV->setPort(m_stServerConfig.usPort);
    m_stServer.pSV->setThreadNum(m_stServerConfig.usThreadNum);
    m_stServer.pSV->setInterface(m_vcInterface);
    m_stServer.pSV->setTimer(m_stServerConfig.uiTimer); 

    /*完成初始化*/
    iRtn = m_stServer.pSV->Init();
    if (Q_RTN_OK != iRtn)
    {
        Q_SafeDelete(m_stServer.pSV);

        return iRtn;
    }

    /*增加服务器间连接*/
    if (!(m_stServerConfig.lstLinkerInfo.empty()))
    {
        Q_Printf("%s", "add service linker...");
    }

    iRtn = startLinker(m_stServerConfig, m_stServer);
    if (Q_RTN_OK != iRtn)
    {
        Q_SafeDelete(m_stServer.pSV);

        return iRtn;
    }

    /*加入到线程*/
    CServerTask *pTask = new(std::nothrow) CServerTask();
    if (NULL == pTask)
    {
        Q_SafeDelete(m_stServer.pSV);
        Q_LOG(LOGLV_ERROR, "%s", Q_EXCEPTION_ALLOCMEMORY);

        return Q_ERROR_ALLOCMEMORY;
    }

    pTask->setServerHandle(m_stServer.pSV);
    m_objThread.Execute(pTask);
    if (!(m_stServer.pSV->waitForStarted()))
    {
        Q_SafeDelete(m_stServer.pSV);
        Q_LOG(LOGLV_ERROR, "start server on port %d error.", m_stServerConfig.usPort);

        return Q_RTN_FAILE;
    }

    return Q_RTN_OK;
}

int CServerInit::startLinker(ServerInfo &stServerInfo, ServerHandle &stSVHandle)
{
    std::list<ServerLinkerInfo>::iterator itLinker;
    
    //为每个线程添加
    for (unsigned short us = 0; us < stSVHandle.pSV->getThreadNum(); us++)
    {
        std::vector<CServerLinker *> vcTmpSVLinker;
        for (itLinker = stServerInfo.lstLinkerInfo.begin(); stServerInfo.lstLinkerInfo.end() != itLinker; 
            itLinker++)
        {
            CServerLinker *pLinker = new(std::nothrow) CServerLinker();
            if (NULL == pLinker)
            {
                Q_LOG(LOGLV_ERROR, "%s", Q_EXCEPTION_ALLOCMEMORY);

                return Q_ERROR_ALLOCMEMORY;
            }

            pLinker->setLinkerName(itLinker->strLinkerName.c_str());
            pLinker->setPort(itLinker->usPort);
            pLinker->setType(itLinker->iType);
            pLinker->setIp(itLinker->strIP.c_str());
            pLinker->setSockPairEvent(&((stSVHandle.pSV->getServerThreadEvent())[us]));

            vcTmpSVLinker.push_back(pLinker);
            stSVHandle.lstLinker.push_back(pLinker);
        }

        m_vcInterface[us]->setSVLinker(vcTmpSVLinker);
    }

    return Q_RTN_OK;
}

void CServerInit::destroyServer(void)
{
    std::list<CServerLinker *>::iterator itLinker;
    std::vector<CEventInterface * >::iterator itInterface;

    for (itLinker = m_stServer.lstLinker.begin(); m_stServer.lstLinker.end() != itLinker; itLinker++)
    {
        Q_SafeDelete(*itLinker);
    }

    Q_SafeDelete(m_stServer.pSV);

    for (itInterface = m_vcInterface.begin(); m_vcInterface.end() != itInterface; itInterface++)
    {
        Q_SafeDelete(*itInterface);
    }

    m_vcInterface.clear();
}

void CServerInit::initSampleLog(void)
{
    std::string strLogPath;
    std::string strLogName;
    int iMaxSize = 5*1024*1024;
    int iPriority = 600;

    g_pSampleLoger = new(std::nothrow) CSampleLoger();
    if (NULL == g_pSampleLoger)
    {
        Q_Printf("%s", Q_EXCEPTION_ALLOCMEMORY);

        return;
    }

    strLogPath = Q_FormatStr("%s%s%s", g_acModulPath, LOG_FOLDER, Q_PATH_SEPARATOR);    

    m_objXmlNode = m_objXmlDoc.child("QServer").child("LogConfig");
    if (!m_objXmlNode.empty())
    {
        strLogName = m_objXmlNode.child_value("LogName");
        Q_Printf("log file %s.", strLogName.c_str());

        iMaxSize = atoi(m_objXmlNode.child_value("MaxSize"));
        Q_Printf("log file size %d.", iMaxSize);

        iPriority = atoi(m_objXmlNode.child_value("Priority"));
        Q_Printf("log priority %d.", iPriority);
    }

    g_pSampleLoger->setLogMaxSize(iMaxSize);    
    g_pSampleLoger->setPriority((LOG_LEVEL)iPriority);
    g_pSampleLoger->setLogFile(std::string(strLogPath + strLogName).c_str());
    g_pSampleLoger->Open();

    g_SampleLogerFD = m_objLog.addLoger(g_pSampleLoger);

    return;
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
        iRtn = m_objServerInit.Start();        
    }
    catch(CException &e)
    {
        Q_Printf("get an exception. code %d, message %s", e.getErrorCode(), e.getErrorMsg());

        iRtn = e.getErrorCode();
    }

    if (Q_RTN_OK != iRtn)
    {
        Service_Exit();
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
    m_objServerInit.Stop(); 

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
#ifndef Q_OS_WIN32
    Q_LOG(LOGLV_INFO, "catch signal %d.", iSigNum);
    Q_Printf("catch signal %d.", iSigNum);

    g_objExitMutex.Lock();
    g_objExitCond.Signal();
    Q_LOG(LOGLV_INFO, "%s", "signal stop service.");
    g_objExitMutex.unLock();
#endif
}
