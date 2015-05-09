
#ifndef Q_EVENTBUFFER_H_
#define Q_EVENTBUFFER_H_

#include "Macros.h"

/*
bufferevent操作类
*/
class CEventBuffer
{
public:
    CEventBuffer(void);
    ~CEventBuffer(void);

    /*根据传入的bufferevent获取读写buffer及socket句柄*/
    int setBuffer(struct bufferevent *pBev);
    /*获取bufferevent句柄*/
    struct bufferevent *getBuffer(void);    

    /*获取读缓冲区中数据的长度*/
    size_t getTotalLens(void) const;
    /*从读缓冲区中取出iLens个字节的数据*/
    char *readBuffer(const size_t &iLens);
    /*删除读缓冲区的数据*/
    int delBuffer(const size_t &iLens);
    /*将数据加入写缓冲区*/
    int writeBuffer(const char *pszBuffer, const size_t &iLens);
    /*将读缓冲区中的数据移动到写缓冲区*/
    int moveBufferData(void);
    /*查找*/
    struct evbuffer_ptr Search(const char *pszWhat, size_t iLens, const struct evbuffer_ptr *pStart = NULL);
    /*获取socket句柄*/
    Q_SOCK getFD(void) const;

private:
    struct bufferevent *m_pBev; 
    struct evbuffer *m_pReadBuffer;
    struct evbuffer *m_pWriteBuffer;
    Q_SOCK m_FD;
};

#endif//Q_EVENTBUFFER_H_
