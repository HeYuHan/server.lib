#pragma once
#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__
#include "common.h"
#include <curl/curl.h>
BEGIN_NS_CORE
class HttpClient
{
	enum 
	{
		HTTP_CLIENT_GET,
		HTTP_CLIENT_POST
	};
public:
	HttpClient();
	~HttpClient();
public:

	bool Post(const char* url, const char* data, const char* contentType = 0);
	bool Get(const char* url);
	bool Request(const char* host, int method, const char **header, int header_length, const char* query, const char * data);
private:
	static int WriteCallback(void* buffer, size_t size, size_t nmemb, void* content);
public:
	const char* m_Error;
	int m_ResultCode;
	std::string m_Content;
private:
	CURL *curl;
};

END_NS_CORE
#endif