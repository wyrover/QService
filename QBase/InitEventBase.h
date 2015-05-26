
#ifndef Q_INITEVENTBASE_H_
#define Q_INITEVENTBASE_H_

#include "Macros.h"

class CInitBase
{
public:
    CInitBase(void);
    ~CInitBase(void);

    struct event_base *getBase(void) const;

private:
    void Init(void);

private:
    struct event_config *m_pCfg;
    struct event_base *m_pBase;
};

#endif//Q_INITEVENTBASE_H_
