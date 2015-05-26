
#include "InitEventBase.h"
#include "Exception.h"

CInitBase::CInitBase(void) : m_pCfg(NULL), m_pBase(NULL)
{
    Init();
}

CInitBase::~CInitBase(void)
{
    try
    {
        if (NULL != m_pCfg)
        {
            event_config_free(m_pCfg);
            m_pCfg = NULL;
        }

        if (NULL != m_pBase)
        {
            event_base_free(m_pBase);
            m_pBase = NULL;
        }
    }
    catch(...)
    {

    }
}

void CInitBase::Init(void)
{
    m_pCfg = event_config_new();
    if (NULL == m_pCfg)
    {
        Q_EXCEPTION(Q_RTN_FAILE, "%s", "event_config_new error.");
    }

#ifdef Q_IOCP
    evthread_use_windows_threads();
    event_config_set_flag(m_pCfg, EVENT_BASE_FLAG_STARTUP_IOCP);
#endif

    m_pBase = event_base_new_with_config(m_pCfg);
    if (NULL == m_pBase)
    {
        Q_EXCEPTION(Q_RTN_FAILE, "%s", "event_base_new error.");
    }

#ifdef Q_IOCP
    Q_Printf("event version %s, using %s", event_get_version(), "IOCP");
#else
    Q_Printf("event version %s, using %s", event_get_version(), event_base_get_method(m_pBase));
#endif
}

struct event_base *CInitBase::getBase(void) const
{
    return m_pBase;
}
