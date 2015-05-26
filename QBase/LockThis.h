
#ifndef Q_LOCKTHIS_H_
#define Q_LOCKTHIS_H_

#include "Mutex.h"

class CLockThis
{
public:
    explicit CLockThis(CQMutex *pMutex);
    ~CLockThis(void);

private:
    CQMutex *m_pMutex;
};

#endif//Q_LOCKTHIS_H_
