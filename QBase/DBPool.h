
#ifndef Q_DBPOOL_H_
#define Q_DBPOOL_H_

#include "QMySQL.h"
#include "Sqlite.h"
#include "Cond.h"

/*数据库连接池*/
class CDBPool
{
public:
    CDBPool(void);
    ~CDBPool(void);

    void setUrl(CDBUrl &objUrl);
    /*数据库连接池最小连接数*/
    void setMinSize(unsigned short usMinPoolSize);
    /*数据库连接池最大连接数*/
    void setMaxSize(unsigned short usMaxPoolSize);
    void setType(DBType emType);
    /*空闲时间(秒)， 超时将回收未使用的额外连接*/
    void setMaxFreeTime(unsigned int usS);

    int Start(void);
    void Stop(void);

    CDBLink *mallocLinker(void);
    void freeLinker(CDBLink *pLinker);

private:
    int getLinker(CDBLink **ppLinker);
    CDBLink *getFromMin(void);
    CDBLink *getFromMax(void);

private:
    long m_lRunFlag;
    CDBUrl m_objUrl;
    unsigned short m_usMinPoolSize;
    unsigned short m_usMaxPoolSize;
    unsigned short m_usMaxLinkerNum;
    DBType m_emType;
    unsigned int m_uiFreeTime;
    std::list<CDBLink *> m_lstMinLinker;
    std::list<struct stBDMaxPoolInfo *> m_lstMaxLinker;
    CQMutex m_objMinMutex;
    CQMutex m_objMaxMutex;
    CQMutex m_objMutex_Exit;
    CCond m_objCond_Exit;
    CQMutex m_objMutex_Start;
    CCond m_objCond_Start;
};

#endif//Q_DBPOOL_H_
