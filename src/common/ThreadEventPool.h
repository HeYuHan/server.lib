#pragma once
#ifndef __THREAD_EVNET_POOL_H__
#define __THREAD_EVNET_POOL_H__
#include <event.h>
#include "ThreadPool2.h"
#include "common.h"
BEGIN_NS_CORE
class EventTask;
class ThreadEventPool
{
	friend class EventTask;
public:
	ThreadEventPool();
	~ThreadEventPool();
public:
	bool Init(unsigned int size);
	struct event_base *Get();
private:
	int m_Size;
	MutexLock m_Lock;
	ThreadPool m_Threads;
	EventTask *m_AllTask;
	int m_InitedThredCount;
};

extern ThreadEventPool gEventPool;
END_NS_CORE
#endif