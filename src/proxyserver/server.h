#pragma once
#ifndef __PROXY_SERVER_H__
#define __PROXY_SERVER_H__
#include <BaseServer.h>
#include <TcpListener.h>
#include <ThreadPool2.h>
#include <common.h>
#include "rpcconnection.h"
#include <objectpool.h>
#include "client.h"
#include "channelserver.h"
#include "chatserver.h"
#include "room.h"
USING_NS_CORE
class Server:public BaseServer,public TcpListener
{
public:
	Server();
	~Server();
public:
	virtual bool Init();
	virtual int Run();
public:
	ObjectPool<Room> m_CachedRooms;
	ObjectPool<Client> m_CachedClients;
	ObjectPool<Chatclient> m_chatClients;
	NS_VECTOR::vector<Client*> m_OnLineClients;
	Core::MutexLock m_Lock;
public:
	RPCConection m_RPCService;
	ChannelListener m_ChannelListener;
	ChatListener m_ChatListener;
public:
	char m_Addr[65];
	char m_ChannelAddr[65];
	char m_ChatAddr[65];
	char **m_InfoAddrs;
	int m_InfoAddrsLength;
public:
	// ͨ�� TcpListener �̳�
	virtual void OnTcpAccept(evutil_socket_t socket, sockaddr *) override;
	void AddOnlineClient(Client* c);
	void RemoveOnlineClient(Client* c);
	Client* GetClient(uint uid);
	Client* NewClient();
	void FreeClient(uint uid);
	void SetRpcRequest();
	void ClientRpcProxy(Client* c,const char* methodName,int sig,void *request,int request_length);
	void UserLogin(Client* c);
	void PlayerLogin(Client * c);
	void BroadCastRoomChange(Room *room,Core::byte type);
	void WriteRoomList(Client *client);
	void FreeRoom(Room * room);
	void ChatClient(Client *c, const char*methodName, int sig, void *request, int request_length);
	
};
extern Server gServer;
#define RPC_CALL(__NAME__,__SIG__,__HEADER__,__MESSAGE__,__INNERARG__) \
	RpcCall<__NAME__##Request,__NAME__##Response>(&gServer.m_RPCService,__SIG__,__HEADER__,__MESSAGE__,__INNERARG__)

#define RPC_PROXY_CALL(__SIG__,__HEADER__,__MESSAGE__,__MESSAGE_LEGTH__,__INNERARG__) \
	RpcProxyCall(&gServer.m_RPCService,__SIG__,__HEADER__,__MESSAGE__,__MESSAGE_LEGTH__,__INNERARG__)

#define BEGIN_RPC_CALLBACK(__NAME__) \
	void Rpc_##__NAME__##_CallBack(RPCRequestPack<google::protobuf::Message, google::protobuf::Message>* _origin_) {\
		RPCRequestPack<__NAME__##Request,__NAME__##Response> *response = reinterpret_cast<RPCRequestPack<__NAME__##Request,__NAME__##Response>*>(_origin_);
#define END_RPC_CALLBACK() }

#define BEGIN_PROXY_RPC_CALLBACK(__NAME__) \
		void Rpc_##__NAME__##_CallBack(RPCRequestPack<google::protobuf::Message, google::protobuf::Message>* response) {

#endif

