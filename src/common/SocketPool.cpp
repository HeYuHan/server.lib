#include "SocketPool.h"
#include "log.h"
#include "Timer.h"
#include <event.h>
#include "ThreadEventPool.h"
#include <string.h>
#include <algorithm>
#include "tools.h"
BEGIN_NS_CORE
SocketPool gSocketPool;
SocketPool::SocketPool():m_UseThread(false)
{

}

SocketPool::~SocketPool()
{
}


void ReadEvent(bufferevent * bev, void * arg)
{ 
	//bufferevent_lock(bev);
	SocketPoolClinet * c = static_cast<SocketPoolClinet*>(arg);
	
	if (c)
	{
		if (!c->m_HandShake)
		{
			c->OnRead();
			goto result;
		}
		if (!c->m_Handle)
		{
			struct evbuffer * input = bufferevent_get_input(bev);
			size_t len = evbuffer_get_length(input);
			int uid_len = sizeof(uint);
			if (len < (size_t)uid_len)
			{
				goto result;
			}
			else
			{
				uint read_uid = 0;
				bufferevent_read(bev, &read_uid, uid_len);
				bool recon = false;
				if (read_uid > 0)
				{
					log_info("reconnect uid %d", read_uid);
					c->m_Pool->Lock();
					std::vector<ISocketClient*>::iterator iter = c->m_Pool->m_DisconnectedClients.begin();
					for (; iter != c->m_Pool->m_DisconnectedClients.end(); iter++)
					{
						ISocketClient *sc = *iter;
						if (!sc)
						{
							iter = c->m_Pool->m_DisconnectedClients.erase(iter);
							if (iter == c->m_Pool->m_DisconnectedClients.end())
							{
								break;
							}
						}
						else if(sc->uid == read_uid)
						{
							recon = true;
							c->m_Handle = sc;
							//发给客户端新的UID
							c->Send(&sc->uid, uid_len);
							c->OnReconnected();
							c->m_Pool->m_DisconnectedClients.erase(iter);
							//if (len > uid_len)c->m_Handle->OnMessage();
							c->m_Pool->UnLock();
							goto result;
						}
					}
					c->m_Pool->UnLock();
				}
				if (!recon)
				{
					c->m_Pool->m_OnAcceptNewClient(c, c->m_Pool->m_UserData);
				}
				if (c->m_Handle)
				{
					//发给客户端新的UID
					c->Send(&c->m_Handle->uid, uid_len);
					c->OnConnected();
					//if (len > uid_len)c->m_Handle->OnMessage();
				}
				else
				{
					c->Disconnect();
				}
			}
		}
		else
		{
			c->OnRead();
		}
	}
result:
	//bufferevent_unlock(bev);
	return;
}

void WriteEvent(bufferevent * bev, void * arg)
{
	SocketPoolClinet * c = static_cast<SocketPoolClinet*>(arg);
	if (c)c->OnWrite();
}

void SocketEvent(bufferevent * bev, short events, void * arg)
{
	if (events & BEV_EVENT_EOF) {
		//printf("connection closed\n");
	}
	else if (events & BEV_EVENT_ERROR) {
		//printf("some other error\n");
	}
	//int fd = bufferevent_getfd(bev);
	//log_info("socket error call event:%d fd:%d", events, fd);
	SocketPoolClinet * c = static_cast<SocketPoolClinet*>(arg);
	if (c)
	{
	
		c->Disconnect();
	}
}

bool SocketPool::Init(uint size,bool hand_shake_uid,OnAcceptNewClient accept_callback,bool useThread, void* user_data)
{
	do 
	{
		m_HandShake = hand_shake_uid;
		m_OnAcceptNewClient = accept_callback;
		m_UseThread = useThread;
		m_UserData = user_data;
		MainThreadMessageQueue::Init();
		if (!m_CachedClients.Initialize(size))break;
		return true;
	} while (0);
	return false;
}

bool SocketPool::Connect(sockaddr *sock_in,int sock_len)
{
	/*sockaddr_in sock_in;
	memset(&sock_in, 0, sizeof(sock_in));
	ParseSockAddr(sock_in, addr);*/
	evutil_socket_t fd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 == connect(fd, sock_in, sock_len))
	{
		AcceptClient(fd, sock_in);
		return true;
	}
	int error = evutil_socket_geterror(fd);
	log_error("connect error:%s", evutil_socket_error_to_string(error));
	return false;
}

void SocketPool::AcceptClient(int fd, sockaddr * sock)
{
	SocketPoolClinet *c = m_CachedClients.Allocate();
	if (!c)
	{
		log_error("can not allocate pool client current size:%d", m_CachedClients.Size());
		return;
	}

	if (!m_HandShake)
	{
		this->m_OnAcceptNewClient(c, this->m_UserData);
		if (c->m_Handle)
		{
			c->Init(fd, sock, this, m_HandShake, m_UseThread);
			c->OnConnected();
		}
		else
		{
			c->Disconnect();
		}
	}
	else
	{
		c->Init(fd, sock, this, m_HandShake, m_UseThread);
	}
	

	
}

void SocketPool::RegisterDisconectedClient(ISocketClient * c)
{
	std::vector<ISocketClient*>::iterator iter = std::find(m_DisconnectedClients.begin(), m_DisconnectedClients.end(), c);
	if (iter == m_DisconnectedClients.end())m_DisconnectedClients.push_back(c);
	
}

void SocketPool::UnRegisterDisconectedClient(ISocketClient * c)
{
	std::vector<ISocketClient*>::iterator iter = std::find(m_DisconnectedClients.begin(), m_DisconnectedClients.end(), c);
	if (iter != m_DisconnectedClients.end())m_DisconnectedClients.erase(iter);
}

SocketPoolClinet::SocketPoolClinet():m_Socket(-1),m_BufferEvent(NULL),m_Handle(NULL),m_ThreadEventBase(NULL)
{
}

//int SocketPoolClinet::SetEvent(event_base * base)
//{
//	if (m_BufferEvent)
//	{
//		event_base* this_base = bufferevent_get_base(m_BufferEvent);
//		if (this_base == base)return 0;
//		do {
//			bufferevent *new_buffer = bufferevent_socket_new(base, m_Socket, 0);
//			if (!new_buffer)break;
//			if (0 != evbuffer_add_buffer(bufferevent_get_output(new_buffer), bufferevent_get_output(m_BufferEvent)))break;
//			if (0 != evbuffer_add_buffer(bufferevent_get_input(new_buffer), bufferevent_get_input(m_BufferEvent)))break;
//			bufferevent_setcb(new_buffer, ReadEvent, WriteEvent, SocketEvent, this);
//			bufferevent_enable(new_buffer, EV_READ | EV_WRITE | EV_PERSIST);
//			bufferevent_disable(m_BufferEvent, EV_READ | EV_WRITE | EV_PERSIST);
//
//			bufferevent_free(m_BufferEvent);
//			m_BufferEvent = new_buffer;
//			return 0;
//		} while (false);
//		
//		return -1;
//		//return bufferevent_base_set(base, m_BufferEvent);
//	}
//	return -1;
//}

void SocketPoolClinet::Update(float time)
{

}

int SocketPoolClinet::Read(void * data, int size)
{

	if (m_BufferEvent)
	{
		//bool lock_buff = NULL != m_ThreadEventBase;
		//if (lock_buff)bufferevent_lock(m_BufferEvent);
		int ret = bufferevent_read(m_BufferEvent, data, size);
		//if (lock_buff)bufferevent_unlock(m_BufferEvent);
		return ret;
	}
	return 0;
}

int SocketPoolClinet::Send(void * data, int size)
{
	if (m_BufferEvent)
	{
		//bool lock_buff = NULL != m_ThreadEventBase;
		//if (lock_buff)bufferevent_lock(m_BufferEvent);
		/*struct evbuffer * output = bufferevent_get_output(m_BufferEvent);
		size_t len = evbuffer_get_length(output);
		if(len >0)log_error("unsend data size:%d", len);*/
		int ret = bufferevent_write(m_BufferEvent, data, size);
		

		//if (lock_buff)bufferevent_unlock(m_BufferEvent);
		return ret == 0 ? size : 0;
		/*int s = 0;
		do
		{
			s += send(m_Socket, &((char*)data)[s], size - s, 0);
		} while (s < size);
		return size;*/
	}
	return 0;
}

void SocketPoolClinet::OnWrite()
{
	if (m_Handle) {
#if _DEBUG
		if (m_ThreadEventBase)
		{
			//log_info("client onwrite tid:%lu uid:%d", pthread_self(), uid);
		}
#endif
		m_Handle->OnWrite();
	}
}

void SocketPoolClinet::OnConnected()
{
	if (m_Handle) {
#if _DEBUG
		if (m_ThreadEventBase)
		{
			//log_info("client onconnected tid:%lu uid:%d", PthreadSelf(),uid);
		}
#endif
		m_Handle->OnConnected();
	}
}

void SocketPoolClinet::OnDisconnected()
{
	if (m_Handle) {
#if _DEBUG
		if (m_ThreadEventBase)
		{
			//log_info("client disconnected tid:%lu uid:%d", PthreadSelf(), uid);
		}
#endif
		m_Handle->OnDisconnect();
	}
}

void SocketPoolClinet::Disconnect()
{
	
	OnDisconnected();
	m_Pool->m_CachedClients.Free(this->uid);
	if (m_BufferEvent)bufferevent_free(m_BufferEvent);
	if (m_Socket >= 0)evutil_closesocket(m_Socket);
	m_Handle = NULL;
	m_BufferEvent = NULL;
	m_Socket = -1;
}

void SocketPoolClinet::OnReconnected()
{
	if (m_Handle) {
#if _DEBUG
		if (m_ThreadEventBase)
		{
			//log_info("client reconnected tid:%lu uid:%d", PthreadSelf(), uid);
		}
#endif
		m_Handle->OnReconnected(this);
	}
}

void SocketPoolClinet::OnRead()
{
	if (m_Handle)
	{
		if (m_Pool->m_UseThread)
		{
			m_Handle->OnRevcMessage(false);
			this->SendMessageToMainThread(1);
		}
		else
		{
			m_Handle->OnRevcMessage(true);
		}
	}
	
	
}

void SocketPoolClinet::OnThreadMessage(unsigned int id)
{
	if (m_Handle)m_Handle->ParseMessage();
}

void SocketPoolClinet::Init(int fd, sockaddr * sock, SocketPool *pool, bool check_uid, bool use_thread)
{
	m_HandShake = check_uid;
	m_Socket = fd;
	m_Pool = pool;
	memset(&m_SockAddr, 0, sizeof(m_SockAddr));
	memcpy(&m_SockAddr, sock, sizeof(m_SockAddr));
	Event *base = m_Handle ? m_Handle->GetEvent() : NULL;
	if (!base)
	{
		base = use_thread ? (m_ThreadEventBase = gEventPool.Get()) : Timer::GetEventBase();
	}
	m_BufferEvent = bufferevent_socket_new(base, m_Socket, 0| BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(m_BufferEvent, ReadEvent, WriteEvent, SocketEvent, this);
	bufferevent_enable(m_BufferEvent, EV_READ | EV_WRITE | EV_PERSIST);
}

END_NS_CORE