#include "MessageQueue.h"
#include "ThreadPool2.h"
#include "Timer.h"
#include <string.h>
BEGIN_NS_CORE
#define QUEUE_LEN 128
MainThreadMessageQueue gQueue;
bool m_Init = false;
Timer m_Timer;
MutexLock m_Lock;
ThreadCondition m_ListFull;

int m_QueueCount = 0;



struct MessageInfo 
{
	unsigned int id;
	IThreadMessageHadle *handle;
};

MessageInfo m_MessageList[QUEUE_LEN];



void UpdateQueue(float t, void *arg)
{
	gQueue.Update();
}

void MainThreadMessageQueue::RegisterHandle(IThreadMessageHadle * handle, unsigned int id)
{
	EasyMutexLock l(m_Lock);
	if (!m_Init)return;
	while (m_QueueCount == QUEUE_LEN)
	{
		m_ListFull.Wait();
	}
	MessageInfo *info = &m_MessageList[m_QueueCount++];
	info->handle = handle;
	info->id = id;
}


void MainThreadMessageQueue::Update()
{
	EasyMutexLock l(m_Lock);
	if (m_QueueCount > 0)
	{
		bool full = m_QueueCount == QUEUE_LEN;
		for (int i = 0; i < m_QueueCount; i++)
		{
			if (m_MessageList[i].handle)m_MessageList[i].handle->OnThreadMessage(m_MessageList[i].id);
		}
		m_QueueCount = 0;
		if (full)m_ListFull.Notify();
	}
}

void MainThreadMessageQueue::Init()
{
	if (m_Init)return;
	m_Init = true;
	memset(m_MessageList, 0, sizeof(m_MessageList));
	m_Timer.Init(0.00001f, UpdateQueue, NULL, true);
	m_Timer.Begin();
	m_QueueCount = 0;
	m_ListFull.Init(&m_Lock);
}

void IThreadMessageHadle::SendMessageToMainThread(unsigned int id)
{
	gQueue.RegisterHandle(this, id);
}
END_NS_CORE