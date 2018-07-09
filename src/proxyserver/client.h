#pragma once
#ifndef __PROXY_CLIENT_H__
#define __PROXY_CLIENT_H__
#include <SocketPool.h>
#include <NetworkConnection.h>
#include <google/protobuf/message.h>
USING_NS_CORE
class Client:public NetworkStream,public ISocketClient
{
	friend class Server;
public:
	Client();
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
	void WriteProtoBuffer(google::protobuf::Message *message);
public:
	bool IsOnline() { return m_IsOnline; }
	bool IsLogin() { return m_AccountId > 0; }

	void Init();
private:
	void RequestRpc();
public:
	uint uid;
	account_id m_AccountId;
	player_id m_PlayerId;
	char m_Name[64 + 1];
private:
	bool m_IsOnline;
};





#endif // !__PROXY_CLIENT_H__

