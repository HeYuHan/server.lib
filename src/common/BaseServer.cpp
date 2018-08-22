#include "BaseServer.h"
#include <event2/event_struct.h>
#include <event2/util.h>
#include <event2/thread.h>
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
	return ret;
}

int BaseServer::Run()
{
	return Timer::Loop();
	
}
