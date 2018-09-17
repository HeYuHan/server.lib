#include "tools.h"
#include <time.h>
#include <stdlib.h>
#include "log.h"
#include <string.h>
#include "common.h"
#ifndef WIN32
#include <unistd.h>

#endif // LINUX
#include "DR_SHA1.h"
#include "ThreadPool2.h"
USING_NS_CORE
static bool m_RandomFirst = true;
int RandomRange(int a, int b)
{
	if (m_RandomFirst) {
		m_RandomFirst = false;
		srand((unsigned int)time(NULL));
	}
	return (int)((double)rand() / ((RAND_MAX + 1.0) / (b - a + 1.0)) + a);
}
//void ParseJsonValue(Json::Value json, const char* key, int &value)
//{
//	if (!json[key].isNull())value = json[key].asInt();
//	else
//	{
//
//		log_warn("%s config miss use default:%d", key, value);
//	}
//}
//void ParseJsonValue(Json::Value json, const char* key, float &value)
//{
//	if (!json[key].isNull())value = json[key].asDouble();
//	else
//	{
//
//		log_warn("%s config miss use default:%f", key, value);
//	}
//}
//void ParseJsonValue(Json::Value json, const char* key, bool &value)
//{
//	if (!json[key].isNull())value = json[key].asBool();
//	else
//	{
//
//		log_warn("%s config miss use default:%s", key, value ? "true" : "false");
//	}
//}
//void ParseJsonValue(Json::Value json, const char* key, char* str, int len)
//{
//	if (!json[key].isNull())
//	{
//		memset(str, 0, len);
//		int str_len = json[key].asString().size();
//		if (str_len < len)
//		{
//			strcpy(str, json[key].asString().c_str());
//		}
//	}
//	else
//	{
//
//		log_warn("%s config miss use default:%s", key, str);
//	}
//}
bool ParseSockAddr(sockaddr_in & addr, const char * str, bool by_name)
{
	if (str)
	{
		char address[256];
		uint port;

		const char * port_start = strchr(str, ':');
		if (port_start)
		{
			memcpy(address, str, port_start - str);
			address[port_start - str] = 0;
			port = atoi(port_start + 1);

			memset(&addr, 0,sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);

			struct hostent* host = NULL;
			if (by_name && (host = gethostbyname(address)))
			{
				addr.sin_addr.s_addr =   inet_addr(inet_ntoa(*((struct in_addr*)host->h_addr)));
			}
			else
			{
				if (address[0])
					addr.sin_addr.s_addr = inet_addr(address);
			}

			return true;
		}
	}

	return false;
}
void CaculateSha1(const char * text, char * out)
{
	NS_CORE::CSHA1 sha1;
	sha1.Update((unsigned char*)text, strlen(text));
	sha1.Final();
	sha1.GetHash((unsigned char*)out);
}
float DiffTime(timeval &v1, timeval &v2)
{
	float diff_sec = v1.tv_sec - v2.tv_sec;
	float diff_usec = (v1.tv_usec - v2.tv_usec) / 1000000.0f;
	return diff_sec + diff_usec;
}
bool RunAsDaemon()
{
#ifdef LINUX

	// ignore signals
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	return daemon(1, 0) == 0;
#endif // LINUX
	return true;
}
unsigned long long PthreadSelf()
{
#ifdef WIN32
	return::GetCurrentThreadId();
#else
	 return pthread_self();
#endif
}
extern unsigned long long MainThreadID;
bool IsMainThread()
{
	return PthreadSelf() == MainThreadID;
}
