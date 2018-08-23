#pragma once
#ifndef __SOCKET_POOL_H__
#define __SOCKET_POOL_H__
#include "objectpool.h"
#include "common.h"
#include <event2/bufferevent.h>
#include <vector>
#include <NetworkConnection.h>
#include "ThreadPool2.h"
#include "MessageQueue.h"
BEGIN_NS_CORE
class SocketPoolClinet;
class ISocketClient
{
	friend class SocketPoolClinet;
public:
	Core::uint uid;
protected:
	virtual void OnWrite() = 0;
	virtual void OnRevcMessage(bool parse) = 0;
	virtual void ParseMessage() = 0;
	virtual void OnDisconnect() = 0;
	virtual void OnConnected() = 0;
	virtual Core::Event* GetEvent() { return NULL; }
	virtual void OnReconnected(SocketPoolClinet*) = 0;
	
};
class SocketPool;
class SocketPoolClinet:public NetworkConnection, protected IThreadMessageHadle
{
private:
	enum
	{
		THREAD_ID_CONNECTED=1, THREAD_ID_RECONNECTED,THREAD_ID_MESSAGE,THREAD_ID_DISCONNECTED
	};
public:
	SocketPoolClinet();
public:
	uint uid;
	int m_Socket;
	bool m_HandShake;
	sockaddr_in m_SockAddr;
	bufferevent *m_BufferEvent;
	ISocketClient *m_Handle;
	SocketPool *m_Pool;
public:
	//virtual int SetEvent(struct event_base *base);
	virtual void Update(float time);
	virtual int Read(void* data, int size);
	virtual int Send(void* data, int size);
	virtual void OnWrite();
	virtual void OnConnected();
	virtual void OnDisconnected();
	virtual void Disconnect();
	virtual void OnReconnected();
	virtual void OnRead();
	virtual void OnThreadMessage(unsigned int id);
	virtual int GetSocket() { return m_Socket; }
public:
	void Init(int fd, sockaddr * sock, SocketPool *pool,bool check_uid,bool use_thread);
private:
	event_base *m_ThreadEventBase;
};
typedef void(*OnAcceptNewClient)(SocketPoolClinet*,void* user_data);
class SocketPool:public ThreadObject
{
public:
	SocketPool();
	~SocketPool();
public:
	bool Init(uint size, bool hand_shake_uid, OnAcceptNewClient accept_callback, bool useThread,void* user_data);
	bool Connect(sockaddr *addr,int sock_len);
	void AcceptClient(int fd, sockaddr * sock);
	void RegisterDisconectedClient(ISocketClient *c);
	void UnRegisterDisconectedClient(ISocketClient *c);

public:
	ObjectPool<SocketPoolClinet> m_CachedClients;
	std::vector<ISocketClient*> m_DisconnectedClients;
	OnAcceptNewClient m_OnAcceptNewClient;
	bool m_UseThread;
	bool m_HandShake;
	void* m_UserData;
};

extern SocketPool gSocketPool;
END_NS_CORE
#endif