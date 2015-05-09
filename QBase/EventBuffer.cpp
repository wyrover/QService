
#include "EventBuffer.h"

CEventBuffer::CEventBuffer(void) : m_pBev(NULL), m_pReadBuffer(NULL),
    m_pWriteBuffer(NULL), m_FD(Q_INVALID_SOCK)
{

}

CEventBuffer::~CEventBuffer(void)
{
    m_pBev = NULL;
    m_pReadBuffer = NULL;
    m_pWriteBuffer = NULL;
}

/************************************************************************
* Function name:setBuffer
* Description  :根据传入的bufferevent获取读写buffer及socket句柄
* IN           :pBev --bufferevent
* OUT          :NONE
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int CEventBuffer::setBuffer(struct bufferevent *pBev)
{
    if (NULL == pBev)
    {
        m_pBev = NULL;
        m_pReadBuffer = NULL;
        m_pWriteBuffer = NULL;
        m_FD = Q_INVALID_SOCK;

        return Q_ERROR_NULLPOINTER;
    }

    m_pBev = pBev;

    //获取读buffer
    m_pReadBuffer = bufferevent_get_input(m_pBev);
    if (NULL == m_pReadBuffer)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return Q_ERROR_NULLPOINTER;
    }

    //获取写buffer
    m_pWriteBuffer = bufferevent_get_output(m_pBev);
    if (NULL == m_pWriteBuffer)
    {
        Q_Printf("%s", Q_EXCEPTION_NULLPOINTER);

        return Q_ERROR_NULLPOINTER;
    }

    //获取socket句柄
    m_FD = bufferevent_getfd(m_pBev);
    if (Q_INVALID_SOCK == m_FD)
    {
        Q_Printf("%s", "invalid socket handle.");

        return Q_ERROR_INVALIDSOCK;
    }

    return Q_RTN_OK;
}

/************************************************************************
* Function name:getBuffer
* Description  :获取bufferevent句柄
* IN           :NONE
* OUT          :NONE
* Return       :bufferevent *
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
struct bufferevent *CEventBuffer::getBuffer(void)
{
    return m_pBev;
}

/************************************************************************
* Function name:getFD
* Description  :获取socket句柄
* IN           :NONE
* OUT          :NONE
* Return       :Q_SOCK
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
Q_SOCK CEventBuffer::getFD(void) const
{
    return m_FD;
}

/************************************************************************
* Function name:getTotalLens
* Description  :获取读缓冲区中数据的长度
* IN           :NONE
* OUT          :NONE
* Return       :读缓冲区数据长度
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
size_t CEventBuffer::getTotalLens(void) const
{
    if (NULL == m_pReadBuffer)
    {
        return Q_INIT_NUMBER;
    }

    return evbuffer_get_length(m_pReadBuffer);
}

/************************************************************************
* Function name:readBuffer
* Description  :从读缓冲区中取出指定数量的数据
* IN           :iLens --要读取的字节数
* OUT          :NONE
* Return       :char *
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
char *CEventBuffer::readBuffer(const size_t &iLens)
{
    if (NULL == m_pReadBuffer)
    {
        return NULL;
    }

    return (char*)evbuffer_pullup(m_pReadBuffer, (ev_ssize_t)iLens);
}

/************************************************************************
* Function name:delBuffer
* Description  :删除读缓冲区的数据
* IN           :iLens --要删除的字节数
* OUT          :NONE
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int CEventBuffer::delBuffer(const size_t &iLens)
{
    if (NULL == m_pReadBuffer)
    {
        return Q_RTN_OK;
    }

    return evbuffer_drain(m_pReadBuffer, iLens);
}

/************************************************************************
* Function name:writeBuffer
* Description  :将数据写入写缓冲区
* IN           :pszBuffer --要写入的数据  iLens --字节数
* OUT          :NONE
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int CEventBuffer::writeBuffer(const char *pszBuffer, const size_t &iLens)
{
    if (NULL == pszBuffer
        || NULL == m_pWriteBuffer)
    {
        return Q_ERROR_NULLPOINTER;
    }

    return evbuffer_add(m_pWriteBuffer, pszBuffer, iLens);
}

/************************************************************************
* Function name:moveBufferData
* Description  :将读缓冲区中的数据移动到写缓冲区
* IN           :NONE
* OUT          :NONE
* Return       :Q_RTN_OK --成功 其他 --失败
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
int CEventBuffer::moveBufferData(void)
{
    if (NULL == m_pWriteBuffer
        || NULL == m_pReadBuffer)
    {
        return Q_ERROR_NULLPOINTER;
    }

    return evbuffer_add_buffer(m_pWriteBuffer, m_pReadBuffer);
}

struct evbuffer_ptr CEventBuffer::Search(const char *pszWhat, size_t iLens, const struct evbuffer_ptr *pStart)
{
    return evbuffer_search(m_pReadBuffer, pszWhat, iLens, pStart);
}
