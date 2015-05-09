
#ifndef Q_LOGERER_H_
#define Q_LOGERER_H_

#include "Macros.h"

class CLoger
{
public:
    CLoger(void){};    
    virtual ~CLoger(void){};

    virtual void Write(const char *pszMsg, const size_t iLens) = 0;
    virtual bool Check(void)
    {
        return true;
    };
};

#endif
