/*****************************************************************************
 * Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
 * svn:http://asuraserver.googlecode.com/svn/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "Buffer.h"
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
    Q_SafeDelete(m_pBuffer);
}

const char *CBuffer::getBuffer(void)
{
    return m_pBuffer;
}

const size_t CBuffer::getLens(void)
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

        Q_SafeDelete(m_pBuffer);
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
