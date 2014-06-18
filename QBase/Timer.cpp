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

#include "Timer.h"

CTimer::CTimer(void)
{
#ifdef Q_OS_WIN32    
    if (QueryPerformanceFrequency(&m_Freq))
    {
        m_bUseFrequency = true;
        (void)QueryPerformanceCounter(&m_StartCount);
    }
    else
    {
        m_bUseFrequency = false;
        ftime(&m_BgTime);
    }
#else
    gettimeofday(&m_stStart, NULL);
#endif
}

/************************************************************************
* Function name:reStart
* Description  :重置计时器
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
void CTimer::reStart(void)
{
#ifdef Q_OS_WIN32
    if (m_bUseFrequency)
    {
        (void)QueryPerformanceCounter(&m_StartCount);
    }
    else
    {
        ftime(&m_BgTime);
    }    
#else
    gettimeofday(&m_stStart, NULL);
#endif
}

/************************************************************************
* Function name:Elapsed
* Description  :返回流逝的时间(ms)
* IN           :NONE
* OUT          :NONE
* Return       :double
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
double CTimer::Elapsed(void)
{
#ifdef Q_OS_WIN32
    if (m_bUseFrequency)
    {
        (void)QueryPerformanceCounter(&m_EndCount);

        return ((double)(m_EndCount.QuadPart - m_StartCount.QuadPart) 
            / (double)m_Freq.QuadPart) * 1000.0;
    }
    else
    {
        ftime(&m_EndTime);

        return ((double)(m_EndTime.time - m_BgTime.time) * 1000.0 + 
            (double)(m_EndTime.millitm - m_BgTime.millitm));
    }
#else
    gettimeofday(&m_stEnd, NULL);

    return 1000.0 * (double)(m_stEnd.tv_sec - m_stStart.tv_sec) + 
        (double)(m_stEnd.tv_usec - m_stStart.tv_usec) / 1000.0;
#endif
}
