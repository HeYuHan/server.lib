//#include "HttpClient.h"
//#include <event2/http.h>
//#include <RakString.h>
//#include "Timer.h"
//#include "log.h"
//#include <string.h>
//
//BEGIN_NS_CORE
//HttpClient::HttpClient():curl(NULL)
//{
//}
//
//HttpClient::~HttpClient()
//{
//	if (curl)
//	{
//		curl_easy_cleanup(curl);
//		curl = NULL;
//	}
//}
//
//bool HttpClient::Post(const char * url, const char * data, const char * contentType)
//{
//	const char *header[2] =
//	{
//		"charset:utf-8",
//		contentType
//	};
//	return Request(url, HTTP_CLIENT_POST, header, contentType?2:1, NULL, data);
//}
//
//bool HttpClient::Get(const char * url)
//{
//	const char *header[1] =
//	{
//		"charset:utf-8"
//	};
//	return Request(url,HTTP_CLIENT_GET,header,1,NULL,NULL);
//}
//
//bool HttpClient::Request(const char* url,int method, const char **header,int header_length,const char* query, const char * data)
//{
//	CURLcode res = CURLE_OK;
//	if(!curl)curl = curl_easy_init();
//	if (!curl)return false;
//	m_ResultCode = 0;
//	m_Error = NULL;
//	m_Content = "";
//	switch (method)
//	{
//	case HTTP_CLIENT_GET:
//	{
//		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
//		break;
//	}
//	case HTTP_CLIENT_POST:
//	{
//		curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
//		if(data)curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
//		break;
//	}
//	default:
//		break;
//	}
//	curl_easy_setopt(curl, CURLOPT_URL, url);
//	//curl_easy_setopt(curl, CURLOPT_PORT, port);
//	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
//	curl_easy_setopt(curl, CURLOPT_SSLVERSION, 4);
//	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
//	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
//	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
//	if (header_length > 0)
//	{
//		curl_slist *plist = NULL;
//		for (int i = 0; i < header_length; i++)
//		{
//			plist = curl_slist_append(plist, header[i]);
//		}
//		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);
//	}
//
//
//
//	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5000);
//	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10000);
//	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
//
//	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClient::WriteCallback);
//	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)this);
//	res = curl_easy_perform(curl);
//
//	m_Error = curl_easy_strerror(res);
//
//	//http返回码
//	long lResCode = 0;
//	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &lResCode);
//	m_ResultCode = lResCode;
//	if (CURLE_OK != res || 200 != lResCode)
//	{
//		////curl传送失败
//		//if (CURLE_OPERATION_TIMEOUTED == res)
//		//{
//		//	nCode = 1;   //超时返回1
//		//}
//		//else
//		//{
//		//	nCode = -1;    //其它错误返回-1
//		//}
//		log_error("curl send msg error: pRes=%s, lResCode=%ld \n", m_Error, lResCode);
//	}
//	return res == CURLE_OK;
//
//}
//int HttpClient::WriteCallback(void * buffer, size_t size, size_t nmemb, void * arg)
//
//{
//	HttpClient *client = (HttpClient*)arg;
//	client->m_Content.append((char*)buffer, size*nmemb);
//	return size*nmemb;
//}
//END_NS_CORE