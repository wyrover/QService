
#include "LockThis.h"

CLockThis::CLockThis(CQMutex *pMutex) : m_pMutex(pMutex)
{
    m_pMutex->Lock();
}

CLockThis::~CLockThis(void)
{
    m_pMutex->unLock();
}
