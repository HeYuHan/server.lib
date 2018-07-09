#pragma once
#ifndef __CHANNEL_SERVER_H__
#define __CHANNEL_SERVER_H__
#include <TcpListener.h>
#include <SocketPool.h>
#include <common.h>
#include <objectpool.h>
USING_NS_CORE


namespace ChannelInner
{
	typedef enum
	{
		CM_CHANNEL_INFO=1,
		CM_CHANNEL_STATE,
		CM_CHANNEL_RPC,
	}ChannelMessage;
	typedef enum
	{
		SM_CHANNEL_RPC=1
	}ProxyMessage;
}
class ChannelClient:public NetworkStream, public ISocketClient
{
	friend class ChannelListener;
public:
	ChannelClient();
	~ChannelClient();
public:
	// 通过 NetworkStream 继承
	virtual void OnMessage() override;
	virtual bool ThreadSafe() override;

	// 通过 ISocketClient 继承
	virtual uint GetUid() override;
	virtual void OnWrite() override;
	virtual void OnRevcMessage() override;
	virtual void OnDisconnect() override;
	virtual void OnConnected() override;
	virtual void OnReconnected(SocketPoolClinet *) override;
private:
	ChannelListener *m_Listener;
public:
	void Init();
	void SetInfo();
	void SyncState();
	void RpcRequest();
public:
	uint uid;
	char m_InnerAddr[64 + 1];
	char m_OuterAddr[64 + 1];

};


class ChannelListener:public TcpListener
{
public:
	ChannelListener();
	~ChannelListener();

public:


	// 通过 TcpListener 继承
	virtual void OnTcpAccept(evutil_socket_t socket, sockaddr *) override;
	

	bool Init(int channel_count);
private:
	static void OnAcceptPoolClinet(NS_CORE::SocketPoolClinet* c, void *arg);
public:
	NS_CORE::SocketPool m_SocketPool;
	ObjectPool<ChannelClient> m_CachedClient;
};




#endif