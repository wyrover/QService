
#ifndef Q_RWLOCK_H_
#define Q_RWLOCK_H_

#include "Macros.h"

#ifdef Q_OS_WIN

//¶ÁÐ´Ëø,Ê§°ÜÓÐÒì³£Å×³ö
class CRWLock
{
public:
    CRWLock(void);
    ~CRWLock(void);

    /*¶ÁËø*/
    void readLock(void);
    /*Ð´Ëø*/
    void writeLock(void);
    /*½âËø*/
    void unLock(void);

private:
    CRITICAL_SECTION m_Write_Lock;
    CRITICAL_SECTION m_Read_Lock;
    volatile LONG m_lReader_Count;
    volatile bool m_bEnter_Writer;
    enum emReadLockCount
    {
        ReadLock_UnLock = 0,
        ReadLock_Lock = 1
    };
};

#else

class CRWLock
{
public:
    CRWLock(void);
    ~CRWLock(void);

    void readLock(void);
    void writeLock(void);
    void unLock(void);

private:
    pthread_rwlock_t m_RWLock;
};

#endif

#endif//Q_RWLOCK_H_
