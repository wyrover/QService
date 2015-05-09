
#ifndef HTTP_CURL_H_
#define HTTP_CURL_H_

#include "Macros.h"

class CHttpClient
{
public:
	CHttpClient(void);
	~CHttpClient(void);

	//POST请求
	std::string Post(const char *const strUrl, const char *const strPost, const char *pszHttpHeader = "");
	//GET请求
	std::string Get(const char *const strUrl, const char *pszHttpHeader = "");
	//POST请求
	std::string Posts(const char *const strUrl, const char *const strPost, const char *pszHttpHeader = "",
        const char * pCaPath = NULL);
	//GET请求,无证书版本
	std::string Gets(const char *const strUrl, const char *pszHttpHeader = "",
        const char * pCaPath = NULL);
	void setDebug(bool bDebug);

private:
    void setDebug(CURL* pCurl);
    struct curl_slist *setHttpHeader(CURL* pCurl, const char *pszHeader);
    void freeHttpList(struct curl_slist *pHttpHeadList);

private:
	bool m_bDebug;
    std::string m_strResponse;
};

#endif//HTTP_CURL_H_
