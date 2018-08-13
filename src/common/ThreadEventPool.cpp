#include "ThreadEventPool.h"
#include "Timer.h"
#include "log.h"
#include "common.h"
#include "tools.h"
BEGIN_NS_CORE
ThreadEventPool gEventPool;



class EventTask:public ThreadTask
{
	friend class ThreadEventPool;

	EventTask():m_EventBase(NULL)
	{

	}
	~EventTask()
	{
		if (NULL != m_EventBase)
		{
			m_LoopTimer.Stop();
			event_base_loopexit(m_EventBase, NULL);
		}
	}
	//static void ReadEvent(bufferevent * bev, void * arg)
	//{
	//}

	//static void WriteEvent(bufferevent * bev, void * arg)
	//{
	//}

	//static void SocketEvent(bufferevent * bev, short events, void * arg)
	//{
	//	if (events & BEV_EVENT_EOF) {
	//		//printf("connection closed\n");
	//	}
	//	else if (events & BEV_EVENT_ERROR) {
	//		//printf("some other error\n");
	//	}
	//	int fd = bufferevent_getfd(bev);
	//	log_info("socket error call event:%d fd:%d", events, fd);
	//}
	static void Update(float t, void *arg)
	{
		log_info("line : %d eventpool loop update tid:%lu ret:%f", __LINE__ , PthreadSelf(), t);
	}
	virtual void Process()
	{
		
		if (m_EventBase == NULL)return;
		m_LoopTimer.Init(60 * 60 * 24, Update, this, true);
		m_LoopTimer.Begin(m_EventBase);
#if _DEBUG
		m_Pool->m_Lock.Lock();
		m_Pool->m_InitedThredCount++;
		log_info("init event pool thread %d tid:%lu", m_Pool->m_InitedThredCount, PthreadSelf());
		m_Pool->m_Lock.Unlock();
#endif
		int ret = event_base_dispatch(m_EventBase);
		/*if (m_EventBase)
		{
			event_base_free(m_EventBase);
			m_EventBase = NULL;
		}*/
		
		log_info("eventpool exit tid:%lu ret:%d", PthreadSelf(), ret);
		
	}
	bool Init(ThreadEventPool *pool)
	{
		if (m_EventBase)
		{
			event_base_loopexit(m_EventBase, NULL);
		}
		m_Pool = pool;
		m_EventBase = event_base_new();
		

		/*evutil_socketpair(AF_INET, SOCK_STREAM, 0, fd);
		m_BufferEvent = bufferevent_socket_new(m_EventBase, fd[0], BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
		bufferevent_setcb(m_BufferEvent, ReadEvent, WriteEvent, SocketEvent, this);
		bufferevent_enable(m_BufferEvent, EV_READ | EV_WRITE | EV_PERSIST);*/
		return m_EventBase != NULL;
	}

private:
	event_base *m_EventBase;
	Timer m_LoopTimer;
	ThreadEventPool *m_Pool;
	//bufferevent *m_BufferEvent;
	//evutil_socket_t fd[2];
};

ThreadEventPool::ThreadEventPool():m_Size(0), m_AllTask(NULL),m_InitedThredCount(0)
{

}

ThreadEventPool::~ThreadEventPool()
{
	Destory();
}

bool ThreadEventPool::Init(unsigned int size)
{
	this->m_Size = size;
	bool ret = m_Threads.Start(size, size);
	if (ret)
	{
		m_AllTask = new EventTask[size];
		for (size_t i = 0; i < size; i++)
		{
			if (!m_AllTask[i].Init(this))
			{
				return false;
			}
			m_Threads.AddTask(&m_AllTask[i]);
		}
	}
	while (m_InitedThredCount < m_Size)
	{
		//ThreadSleep(1);
	}
	log_info("event pool inited thread count %d", m_InitedThredCount);
	return ret;
}

void ThreadEventPool::Destory()
{
	
	if (m_AllTask)delete[] m_AllTask;
	m_AllTask = NULL;
	m_Threads.Stop();
}
struct event_base * ThreadEventPool::Get()
{
	EasyMutexLock l(m_Lock);
	if (this->m_Size == 0)
	{
		this->Init(5);
	}
	/*int index = t%m_Size;
	t++;
	return m_AllTask[index].m_EventBase;*/
	int number = event_base_get_num_events(m_AllTask[0].m_EventBase, EVENT_BASE_COUNT_ADDED);
	event_base *result = m_AllTask[0].m_EventBase;
	for (int i = 1; i < m_Size; i++)
	{
		int n = event_base_get_num_events(m_AllTask[i].m_EventBase, EVENT_BASE_COUNT_ADDED);
		if (n < number) {
			number = n;
			result = m_AllTask[i].m_EventBase;
		}
		//log_info("get pool event base %d , %d", i, n);
	}


	return result;
}
END_NS_CORE