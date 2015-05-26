
#include "BufferMgr.h"
#include "Exception.h"

CBuffer::CBuffer(void) : m_pBuffer(NULL), 
    m_iOffset(Q_INIT_NUMBER), m_iTotalSize(Q_INIT_NUMBER)
{
    (void)New(5 * Q_ONEK);
}

CBuffer::CBuffer(const size_t iInitSize) : m_pBuffer(NULL), 
    m_iOffset(Q_INIT_NUMBER), m_iTotalSize(Q_INIT_NUMBER)
{
    m_iTotalSize = (0 == iInitSize ? 1 : iInitSize);

    (void)New(m_iTotalSize);
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

size_t CBuffer::getTotalLens(void) const
{
    return m_iTotalSize;
}

char *CBuffer::New(const size_t iLens)
{
    if ((NULL == m_pBuffer)
        || (iLens > m_iTotalSize))
    {
        Q_SafeDelete_Array(m_pBuffer);
        m_pBuffer = new(std::nothrow) char[iLens];
        if (NULL == m_pBuffer)
        {
            m_iTotalSize = Q_INIT_NUMBER;

            return NULL;
        }

        m_iTotalSize = iLens;
    }

    return m_pBuffer;
}

char *CBuffer::reNew(const size_t iLens)
{
    if (NULL == m_pBuffer)
    {
        return New(iLens);
    }

    if (iLens <= m_iTotalSize)
    {
        return m_pBuffer;
    }

    /* need more memory */
    char *pTmp = new(std::nothrow) char[iLens];
    if (NULL == pTmp)
    {
        Q_SafeDelete_Array(m_pBuffer);
        m_iTotalSize = Q_INIT_NUMBER;

        return NULL;
    }

    memcpy(pTmp, m_pBuffer, m_iTotalSize);
    Q_SafeDelete_Array(m_pBuffer);
    m_pBuffer = pTmp;
    m_iTotalSize = iLens;

    return m_pBuffer;
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
    size_t iNewSize = m_iOffset + iLens;
    if (NULL == reNew(iNewSize))
    {
        Q_EXCEPTION(Q_ERROR_ALLOCMEMORY, "%s size %u", Q_EXCEPTION_ALLOCMEMORY, iNewSize);
    }

    if (NULL == pBuff)
    {
        Q_Zero(m_pBuffer + m_iOffset, iLens);
    }
    else
    {
        memcpy(m_pBuffer + m_iOffset, pBuff, iLens);
    }
    
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
