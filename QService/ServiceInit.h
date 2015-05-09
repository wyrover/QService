
#ifndef Q_SERVICEINIT_H_
#define Q_SERVICEINIT_H_

#include "DispWithLua.h"

class CServerInit
{
public:
    CServerInit(void)
    {};
    ~CServerInit(void);

    int Start(void);
    void Stop(void);

private:
    struct LinkOther
    {
        unsigned short usPort;
        std::string strIp;
        std::string strName;
    };
    struct ServerConfig
    {
        unsigned short usThreadNum;
        unsigned int uiTimer;
        std::string strScript;

        std::map<unsigned short, std::string> mapTcp;
        std::map<unsigned short, std::string> mapWebSock;
        std::map<unsigned short, std::string> mapHttp;
    };

private:
    void initTxtLog(void);
    bool initDBLog(void);
    bool readConfig(void);
    int initServer(void);
    void readLinkOtherConfig(std::vector<LinkOther> &vcLinkOther) const;

private:
    CThread m_objThread;
    std::string m_strConfig;
    xml_document m_objXmlDoc;
    xml_parse_result m_objXmlResult;
    xml_node m_objXmlNode;
    CLog m_objLog;
    ServerConfig m_stServerConfig;
    CServer m_objServer;
    std::vector<CEventInterface * > m_vcInterface;
};

int Service_InitProgram(void);
int Service_Exit(void);
void SigHandEntry(int iSigNum);

#endif//Q_SERVICEINIT_H_
