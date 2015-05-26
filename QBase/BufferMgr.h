
#ifndef Q_BUFFER_H_
#define Q_BUFFER_H_

#include "Macros.h"

/*
数据打包
*/
class CBuffer
{
public:
    CBuffer(void);
    /*iInitSize 默认buffer大小*/
    explicit CBuffer(const size_t iInitSize);
    ~CBuffer(void);

    /*添加信息进buffer  pBuff 为NULL以0填充*/
    void pushBuff(const void *pBuff, const size_t &iLens);
    /*重置buffer偏移标志*/
    void reSet(void);

    /*返回buffer*/
    const char *getBuffer(void) const;
    /*获取buffer长度*/
    const size_t getLens(void) const;
    /*获取总长度*/
    size_t getTotalLens(void) const;

    //New
    char *New(const size_t iLens);
    //reNew
    char *reNew(const size_t iLens);

private:
    char *m_pBuffer;
    size_t m_iOffset;
    size_t m_iTotalSize;
};

#endif//Q_BUFFER_H_
