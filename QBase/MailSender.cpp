
#include "MailSender.h"
#include "File.h"
#include "json/json.h"

SINGLETON_INIT(CMailSender)
CMailSender objMailSender;

CMailSender::CMailSender(void) : m_objMailer()
{

}

CMailSender::~CMailSender(void)
{

}

void CMailSender::setServer(const std::string &strServer)
{
    (void)m_objMailer.setserver(strServer);
}

void CMailSender::setAuthType(const enum jwsmtp::mailer::authtype &emType)
{
    m_objMailer.authtype(emType);
}

void CMailSender::setSender(const std::string &strSender)
{
    (void)m_objMailer.setsender(strSender);
}

void CMailSender::setUserName(const std::string &strUserName)
{
    m_objMailer.username(strUserName);
}

void CMailSender::setPassWord(const std::string &strPassWord)
{
    m_objMailer.password(strPassWord);
}

void CMailSender::onMainRead(CEventBuffer *pEventBuffer)
{
    while(true)
    {
        const char *pszBuf = m_objTcpParser.parsePack(pEventBuffer);
        if (NULL == pszBuf)
        {
            return;
        }

        std::string strMail(pszBuf, m_objTcpParser.getBufLens());

        m_objMailer.reset();
        if (Q_RTN_OK == parseMail(strMail))
        {
            m_objMailer.send();
            Q_Printf("%s", m_objMailer.response().c_str());
        }

        pEventBuffer->delBuffer(m_objTcpParser.getParsedLens());
    }
}

int CMailSender::parseMail(std::string &strMail)
{
    Json::Reader objJsonReader;
    Json::Value objJsonRoot;

    if (!objJsonReader.parse(strMail, objJsonRoot, false))
    {
        Q_Printf("parse string <%s> error.", strMail.c_str());

        return Q_RTN_FAILE;
    }
    
    //主题
    std::string strSubject = objJsonRoot["Subject"].asString();
    if (!m_objMailer.setsubject(strSubject))
    {
        Q_Printf("%s", "subject is empty.");

        return Q_RTN_FAILE;
    }
    //字符串消息
    std::string strStrMsg = objJsonRoot["StrMsg"].asString();
    if (!strStrMsg.empty())
    {
        (void)m_objMailer.setmessage(strStrMsg);
    }
    //html字符串消息
    std::string strHtmlStrMsg = objJsonRoot["HtmlStrMsg"].asString();
    if (!strHtmlStrMsg.empty())
    {
        (void)m_objMailer.setmessageHTML(strHtmlStrMsg);
    }
    //html文件消息
    std::string strHtmlFileMsg = objJsonRoot["HtmlFileMsg"].asString();
    if (!strHtmlFileMsg.empty())
    {
        (void)m_objMailer.setmessageHTMLfile(strHtmlFileMsg);
    }
    
    //接收者
    Json::Value objRecipient = objJsonRoot["Recipient"];
    if (objRecipient.empty())
    {
        Q_Printf("%s", "recipient is empty.");

        return Q_RTN_FAILE;
    }
    for (Json::ArrayIndex i = 0; i < objRecipient.size(); i++)
    {
        std::string strRecipient = objRecipient[i].asString();
        if (!strRecipient.empty())
        {
            (void)m_objMailer.addrecipient(strRecipient);
        }
    }

    //附件
    Json::Value objAttach = objJsonRoot["Attach"];
    if (objAttach.empty())
    {
        return Q_RTN_OK;
    }

    for (Json::ArrayIndex i = 0; i < objAttach.size(); i++)
    {
        std::string strFile = objAttach[i].asString();
        if (Q_RTN_OK != Q_FileExist(strFile.c_str()))
        {
            Q_Printf("file %s not exist.", strFile.c_str());

            return Q_RTN_FAILE;
        }

        (void)m_objMailer.attach(strFile);
    }

    return Q_RTN_OK;
}

void CMailSender::sendMail(const std::string &strMail)
{
    if (strMail.empty()
        || !getIsRun())
    {
        return;
    }

    size_t iHeadLens = Q_INIT_NUMBER;
    const char *pszHead = m_objTcpParser.createHead(strMail.size(), iHeadLens);
    if (Q_RTN_OK != sendMainMsg(pszHead, iHeadLens))
    {
        return;
    }
    sendMainMsg(strMail.c_str(), strMail.size());
}
