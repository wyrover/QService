
#ifndef Q_LOGERER_H_
#define Q_LOGERER_H_

#include "Macros.h"

class CLoger
{
public:
    CLoger(void) : m_Sock(Q_INVALID_SOCK)
    {

    };    
    virtual ~CLoger(void){};

    void setSock(Q_SOCK sock)
    {
        m_Sock = sock;
    };
    Q_SOCK getSock(void)
    {
        return m_Sock;
    };

    virtual void Write(const char *pszMsg, const size_t iLens) = 0;
    virtual bool Check(void)
    {
        return true;
    };

private:
    Q_SOCK m_Sock;
};

#endif
