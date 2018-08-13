#pragma once
#ifndef __PROXY_CLIENT_H__
#define __PROXY_CLIENT_H__
#include <SocketPool.h>
#include <NetworkConnection.h>
#include <ClientData.pb.h>
#include <AllMessagePackages.pb.h>
#include <ThreadPool2.h>
USING_NS_CORE
class Room;
class Client:public NetworkStream,public ISocketClient
{
	friend class Server;
public:
	Client();
public:
	// Í¨¹ý NetworkStream ¼Ì³Ð
	virtual void OnMessage() override;
	virtual bool ThreadSafe() override;

	virtual void OnWrite() override;
	virtual void OnRevcMessage() override;
	virtual void OnDisconnect() override;
	virtual void OnConnected() override;
	virtual void OnReconnected(SocketPoolClinet *) override;
public:
	bool IsOnline();
	bool IsLogin() { return m_AccountId > 0; }
	void Init();
private:
	void RequestRpc();
	void RequestCreateRoom();
	void RequestRoomList();
	void RequestOBRoomList();
	void RequestRoomInfo();
	void RequestEnterRoom();
	void RequestLeaveRoom();
	void RequestStartGame();
	void RequestChangeEquip();
	
public:
	void ResponseError(ushort error);
	void WriteClientInfo(NetworkStream *stream);
public:
	Proto::Message::Equip m_Equip; 
	//Proto::Message::RoomCreateOption m_RoomCreateOption;
	account_id m_AccountId;
	player_id m_PlayerId;
	Room *m_OwnerRoom;
	Core::byte m_State;
	char m_Name[64 + 1];
	Proto::Message::ProtoPlayerInfo m_ClientInfo;
	bool m_IsOnline;
};





#endif // !__PROXY_CLIENT_H__

