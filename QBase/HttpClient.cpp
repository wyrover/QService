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
extern "C"
{
    #include "curl.h"
}

CHttpClient::CHttpClient(void) :m_bDebug(false)
{

}

CHttpClient::~CHttpClient(void)
{

}

static void OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
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
		return Q_RTN_FAILE;
	}

	char* pData = (char*)buffer;
	str->append(pData, size * nmemb);

	return nmemb;
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
	std::string strResponse;
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		Q_Printf("%s", "curl_easy_init failed");
		return NULL;
	}

	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	curl_easy_setopt(curl, CURLOPT_URL, strUrl);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8);

	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	return strResponse;
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
	std::string strResponse;
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		Q_Printf("%s", "curl_easy_init failed");
		return NULL;
	}

	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	curl_easy_setopt(curl, CURLOPT_URL, strUrl);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8);

	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	return strResponse;
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
	std::string strResponse;
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		Q_Printf("%s", "curl_easy_init failed");
		return NULL;
	}

	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	curl_easy_setopt(curl, CURLOPT_URL, strUrl);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//缺省情况就是PEM，所以无需设置，另外支持DER
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8);

	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	return strResponse;
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
	std::string strResponse;
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		Q_Printf("%s", "curl_easy_init failed");
		return NULL;
	}

	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	curl_easy_setopt(curl, CURLOPT_URL, strUrl);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8);

	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	return strResponse;
}

void CHttpClient::setDebug(bool bDebug)
{
	m_bDebug = bDebug;
}
