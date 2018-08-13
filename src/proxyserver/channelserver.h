#pragma once
#ifndef __CHANNEL_SERVER_H__
#define __CHANNEL_SERVER_H__
#include <TcpListener.h>
#include <SocketPool.h>
#include <common.h>
#include <objectpool.h>
USING_NS_CORE
class Room;
class ChannelListener;
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
	virtual void OnWrite() override;
	virtual void OnRevcMessage() override;
	virtual void OnDisconnect() override;
	virtual void OnConnected() override;
	virtual void OnReconnected(SocketPoolClinet *) override;
private:
	ChannelListener *m_Listener;
public:
	bool IsValid();
	void Init();
	void SetInfo();
	void SyncState();
	void RpcRequest();
	int GetRate();
	void OnStartGame();
	void OnChangeRoomState();
	void ValidateEnterRoom();
	void LeaveRoom();
	void FreeRoom(uint roomid);
public:
	char m_InnerAddr[64 + 1];
	char m_OuterAddr[64 + 1];
	int m_RoomCount;
	int m_RoomMaxCount;
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
	ChannelClient *GetCanUseChannelRoom();
	void FreeRoom(uint channel,uint room);
private:
	static void OnAcceptPoolClinet(NS_CORE::SocketPoolClinet* c, void *arg);
public:
	NS_CORE::SocketPool m_SocketPool;
	ObjectPool<ChannelClient> m_CachedClient;
};




#endif