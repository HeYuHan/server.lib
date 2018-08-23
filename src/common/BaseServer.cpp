#include "BaseServer.h"
#include <event2/event_struct.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <curl/curl.h>
#include "Timer.h"
#include "tools.h"
unsigned long long MainThreadID;
BaseServer::BaseServer()
{
	
}

BaseServer::~BaseServer()
{
}

bool BaseServer::Init()
{
	MainThreadID = PthreadSelf();
	bool ret = evthread_use_pthreads() == 0;
	//ret = ret && (curl_global_init(CURL_GLOBAL_ALL) == CURLcode::CURLE_OK);
	return ret;
}

int BaseServer::Run()
{
	return Timer::Loop();
	
}
