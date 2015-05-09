
#include "BufferMgr.h"
#include "Exception.h"

CBuffer::CBuffer(void) : m_pBuffer(NULL), 
    m_iOffset(0), m_iTotalSize(Q_ONEK * 2)
{

}

CBuffer::CBuffer(const size_t iInitSize) : m_pBuffer(NULL),
    m_iOffset(0)
{
    assert(iInitSize > 0);
    m_iTotalSize = iInitSize;
}

CBuffer::~CBuffer(void)
{
    Q_SafeDelete_Array(m_pBuffer);
}

const char *CBuffer::getBuffer(void) const
{
    return m_pBuffer;
}

const size_t CBuffer::getLens(void) const
{
    return m_iOffset;
}

/************************************************************************
* Function name:pushBuff
* Description  :添加信息进buffer(可能抛出异常)
* IN           :pBuff --要添加的信息 iLens --长度
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
void CBuffer::pushBuff(const void *pBuff, const size_t &iLens)
{
    if (NULL == m_pBuffer)
    {
        m_pBuffer = new(std::nothrow) char[m_iTotalSize];
        if (NULL == m_pBuffer)
        {
            Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s size %u", Q_EXCEPTION_ALLOCMEMORY, m_iTotalSize);
        }
    }

    if (m_iOffset + iLens > m_iTotalSize)
    {
        char * pTmp = new(std::nothrow) char[m_iOffset + iLens];
        if (NULL == pTmp)
        {
            Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s size %u", Q_EXCEPTION_ALLOCMEMORY, m_iOffset + iLens);
        }

        if (0 != m_iOffset)
        {
            memcpy(pTmp, m_pBuffer, m_iOffset);
        }

        Q_SafeDelete_Array(m_pBuffer);
        m_iTotalSize = m_iOffset + iLens;
        m_pBuffer = pTmp;
    }

    memcpy(m_pBuffer + m_iOffset, pBuff, iLens);
    m_iOffset += iLens;
}

/************************************************************************
* Function name:reSet
* Description  :重置buffer偏移标志
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
void CBuffer::reSet(void)
{
    m_iOffset = Q_INIT_NUMBER;
}
