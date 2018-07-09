#include "BaseServer.h"
#include <event2/event_struct.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <curl/curl.h>
#include "Timer.h"
BaseServer::BaseServer()
{
	
}

BaseServer::~BaseServer()
{
}

bool BaseServer::Init()
{
	bool ret = evthread_use_pthreads() == 0;
	ret = ret && (curl_global_init(CURL_GLOBAL_ALL) == CURLcode::CURLE_OK);
	return ret;
}

int BaseServer::Run()
{
	return Timer::Loop();
	
}
