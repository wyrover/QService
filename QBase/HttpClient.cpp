/*****************************************************************************
* Copyright (c) 2011-2012. Qifu Luo All Rights Reserved.200309129@163.com 
* svn:http://asuraserver.googlecode.com/svn/
* github:https://github.com/QService/QService
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

#include "HttpClient.h"

CHttpClient::CHttpClient(void) :m_bDebug(false)
{

}

CHttpClient::~CHttpClient(void)
{

}

static void OnDebug(CURL *, curl_infotype itype, char * pData, size_t, void *)
{
    switch(itype)
    {
    case CURLINFO_TEXT:
        Q_Printf("[TEXT]%s", pData);
        break;

    case CURLINFO_HEADER_IN:
        Q_Printf("[HEADER_IN]%s", pData);
        break;

    case CURLINFO_HEADER_OUT:
        Q_Printf("[HEADER_OUT]%s", pData);
        break;

    case CURLINFO_DATA_IN:
        Q_Printf("[DATA_IN]%s", pData);
        break;

    case CURLINFO_DATA_OUT:
        Q_Printf("[DATA_OUT]%s", pData);
        break;

    default:
        break;
    }
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if (NULL == str 
        || NULL == buffer)
	{
		return 0;
	}

    size_t iLens = size * nmemb;
	char* pData = (char*)buffer;
	str->append(pData, iLens);

	return iLens;
}

void CHttpClient::setDebug(CURL* pCurl)
{
    (void)curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1);
    (void)curl_easy_setopt(pCurl, CURLOPT_DEBUGFUNCTION, OnDebug);
}

/************************************************************************
* Function name:Post
* Description  :POST请求
* IN           :strUrl 请求的Url地址  strPost 输入参数
* OUT          :NONE
* Return       :返回的内容
* Make By      :lqf/200309129@163.com
* Date Time    :2014/11/18
* Modification 
* ......record :first program
************************************************************************/
std::string CHttpClient::Post(const char *const strUrl, const char *const strPost)
{
    m_strResponse.clear();

	CURL* pCurl = curl_easy_init();
	if (NULL == pCurl)
	{
		Q_Printf("%s", "curl_easy_init failed");
		return m_strResponse;
	}

	if (m_bDebug)
	{
		setDebug(pCurl);
	}

	(void)curl_easy_setopt(pCurl, CURLOPT_URL, strUrl);
	(void)curl_easy_setopt(pCurl, CURLOPT_POST, 1);
	(void)curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strPost);
	(void)curl_easy_setopt(pCurl, CURLOPT_READFUNCTION, NULL);
	(void)curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, OnWriteData);
	(void)curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void *)&m_strResponse);
	(void)curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1);
	(void)curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 3);
	(void)curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 8);

	(void)curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);
    

	return m_strResponse;
}

/************************************************************************
* Function name:Get
* Description  :Get请求
* IN           :strUrl 请求的Url地址
* OUT          :NONE
* Return       :返回的内容
* Make By      :lqf/200309129@163.com
* Date Time    :2014/11/18
* Modification 
* ......record :first program
************************************************************************/
std::string CHttpClient::Get(const char *const strUrl)
{
    m_strResponse.clear();

	CURL* pCurl = curl_easy_init();
	if (NULL == pCurl)
	{
		Q_Printf("%s", "curl_easy_init failed");
		return m_strResponse;
	}

	if (m_bDebug)
	{
		setDebug(pCurl);
	}

	(void)curl_easy_setopt(pCurl, CURLOPT_URL, strUrl);
	(void)curl_easy_setopt(pCurl, CURLOPT_READFUNCTION, NULL);
	(void)curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, OnWriteData);
	(void)curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void *)&m_strResponse);
	(void)curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1);
	(void)curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 3);
	(void)curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 8);

	(void)curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);

	return m_strResponse;
}

/************************************************************************
* Function name:Posts
* Description  :HTTPS POST请求
* IN           :strUrl 请求的Url地址 pCaPath 为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性
* OUT          :NONE
* Return       :返回的内容
* Make By      :lqf/200309129@163.com
* Date Time    :2014/11/18
* Modification 
* ......record :first program
************************************************************************/
std::string CHttpClient::Posts(const char *const strUrl, const char *const strPost, const char * pCaPath)
{
	m_strResponse.clear();

	CURL* pCurl = curl_easy_init();
	if (NULL == pCurl)
	{
		Q_Printf("%s", "curl_easy_init failed");
		return m_strResponse;
	}

	if (m_bDebug)
	{
		setDebug(pCurl);
	}

	(void)curl_easy_setopt(pCurl, CURLOPT_URL, strUrl);
	(void)curl_easy_setopt(pCurl, CURLOPT_POST, 1);
	(void)curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strPost);
	(void)curl_easy_setopt(pCurl, CURLOPT_READFUNCTION, NULL);
	(void)curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, OnWriteData);
	(void)curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void *)&m_strResponse);
	(void)curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		(void)curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, false);
		(void)curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//缺省情况就是PEM，所以无需设置，另外支持DER
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
		(void)curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, true);
		(void)curl_easy_setopt(pCurl, CURLOPT_CAINFO, pCaPath);
	}
	(void)curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 3);
	(void)curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 8);

	(void)curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);

	return m_strResponse;
}

/************************************************************************
* Function name:Gets
* Description  :HTTPS GETS请求,无证书版本
* IN           :strUrl 请求的Url地址  strPost 输入参数 
                pCaPath 为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性
* OUT          :NONE
* Return       :返回的内容
* Make By      :lqf/200309129@163.com
* Date Time    :2014/11/18
* Modification 
* ......record :first program
************************************************************************/
std::string CHttpClient::Gets(const char *const strUrl, const char * pCaPath)
{
	m_strResponse.clear();

	CURL* pCurl = curl_easy_init();
	if (NULL == pCurl)
	{
		Q_Printf("%s", "curl_easy_init failed");
		return m_strResponse;
	}

	if (m_bDebug)
	{
		setDebug(pCurl);
	}

	(void)curl_easy_setopt(pCurl, CURLOPT_URL, strUrl);
	(void)curl_easy_setopt(pCurl, CURLOPT_READFUNCTION, NULL);
	(void)curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, OnWriteData);
	(void)curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void *)&m_strResponse);
	(void)curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		(void)curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, false);
		(void)curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		(void)curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, true);
		(void)curl_easy_setopt(pCurl, CURLOPT_CAINFO, pCaPath);
	}
	(void)curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 3);
	(void)curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 8);

	(void)curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);

	return m_strResponse;
}

void CHttpClient::setDebug(bool bDebug)
{
	m_bDebug = bDebug;
}
