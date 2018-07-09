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
USING_NS_CORE
class Server:public BaseServer,public TcpListener
{
public:
	Server();
	~Server();
public:
	virtual bool Init();
	virtual int Run();
private:
	
	ObjectPool<Client> m_CachedClients;
	NS_VECTOR::vector<Client*> m_OnLineClients;
	Core::MutexLock m_Lock;
public:
	RPCConection m_RPCService;
	ChannelListener m_ChannelListener;
public:
	// Í¨¹ý TcpListener ¼Ì³Ð
	virtual void OnTcpAccept(evutil_socket_t socket, sockaddr *) override;
	void AddOnlineClient(Client* c);
	void RemoveOnlineClient(Client* c);
	Client* GetClient(uint uid);
	Client* NewClient();
	void FreeClient(uint uid);
	void SetRpcRequest();
	void ClientRpcProxy(Client* c,const char* methodName,int sig,void *request,int request_length);
	void Login(Client* c);
	
};
extern Server gServer;
#define RPC_CALL(__NAME__,__SIG__,__HEADER__,__MESSAGE__,__USER_DATA__,__DATA_LEN__) \
	RpcCall<__NAME__##Request,__NAME__##Response>(&gServer.m_RPCService,__SIG__,__HEADER__,__MESSAGE__,__USER_DATA__,__DATA_LEN__)

#define RPC_PROXY_CALL(__SIG__,__HEADER__,__MESSAGE__,__MESSAGE_LEGTH__,__USER_DATA__,__DATA_LEN__) \
	RpcProxyCall(&gServer.m_RPCService,__SIG__,__HEADER__,__MESSAGE__,__MESSAGE_LEGTH__,__USER_DATA__,__DATA_LEN__)

#define BEGIN_RPC_CALLBACK(__NAME__,__PREFIX__) \
	void Rpc_##__NAME__##__PREFIX__##_CallBack(RPCRequestPack<google::protobuf::Message, google::protobuf::Message>* _origin_) {\
		RPCRequestPack<__NAME__##Request,__NAME__##Response> *response = reinterpret_cast<RPCRequestPack<__NAME__##Request,__NAME__##Response>*>(_origin_);
#define END_RPC_CALLBACK() }

#define BEGIN_PROXY_RPC_CALLBACK(__NAME__) \
		void Rpc_##__NAME__##_CallBack(RPCRequestPack<google::protobuf::Message, google::protobuf::Message>* response) {

#endif
