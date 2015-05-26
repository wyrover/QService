
#include "Session.h"

CSession::CSession(void) : m_emSessionType(STYPE_TCP),
    m_emStatus(SESSSTATUS_CLOSED), m_iSessionID(Q_INVALID_ID),
    m_uiPing(Q_INIT_NUMBER)
{
    (void)m_objBuffer.setBuffer(NULL);
}

CSession::~CSession(void)
{

}

void CSession::setType(const SessionType emType)
{
    m_emSessionType = emType;
}

int CSession::getType(void) const
{
    return m_emSessionType;
}

void CSession::setSessionID(const Q_SOCK &uiID)
{
    m_iSessionID = uiID;
}

int CSession::getSessionID(void) const
{
    return (int)m_iSessionID;
}

void CSession::setExterID(const char *pszID)
{
    assert(NULL != pszID);

    m_strID = pszID;
}

const char * CSession::getExterID(void) const
{
    return m_strID.c_str();
}

void CSession::setIDCard(const char *pszID)
{
    assert(NULL != pszID);

    m_strIDCard = pszID;
}

const char * CSession::getIDCard(void) const
{
    return m_strIDCard.c_str();
}

void CSession::setPing(const unsigned int uiPing)
{
    m_uiPing = uiPing;
}

unsigned int CSession::getPing(void) const
{
    return m_uiPing;
}

void CSession::setAccount(const char *pszAccount)
{
    assert(NULL != pszAccount);

    m_strAccount = pszAccount;
}

const char *CSession::getAccount(void) const
{
    return m_strAccount.c_str();
}

void CSession::setStatus(const int iStatus)
{
    m_emStatus = (SessionStatus)iStatus;
}

int CSession::getStatus(void) const
{
    return m_emStatus;
}

CEventBuffer *CSession::getBuffer(void)
{
    return &m_objBuffer;
}

void CSession::Clear(void)
{
    m_emSessionType = STYPE_TCP;
    m_iSessionID = Q_INVALID_ID;
    m_emStatus = SESSSTATUS_CLOSED;
    m_uiPing = Q_INIT_NUMBER;
    m_strID.clear();
    m_strAccount.clear();
    m_strIDCard.clear();
    (void)m_objBuffer.setBuffer(NULL);
}
