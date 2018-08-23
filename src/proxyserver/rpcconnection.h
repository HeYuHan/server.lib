#pragma once
#ifndef __RPC_CONNECTION_H__
#define __RPC_CONNECTION_H__
#include <TcpConnection.h>
#include <SocketPool.h>
#include <Timer.h>
#include <queue>
#include "rpcrequest.h"
#include <ConnectHelper.h>
USING_NS_CORE

class RPCConection;
class RPCClient:public NetworkStream, public ISocketClient
{
public:
	RPCClient();
	~RPCClient();
	virtual uint GetUid();
	virtual bool ThreadSafe();
	virtual void OnMessage();
	virtual void OnRevcMessage(bool parse) override;
	virtual void ParseMessage() override;
	virtual void OnDisconnect();
	virtual void OnConnected();
	virtual void OnReconnected(SocketPoolClinet*);
	virtual void OnWrite();
	virtual Core::Event* GetEvent();
	void Init();
	bool IsConnect();
	

public:
	uint uid;
	RPCConection *m_RPC;
	RPCRequestPack<google::protobuf::Message, google::protobuf::Message> *m_CurrentRequest;
	//Core::MutexLock m_Lock;
private:
	int m_ReuqestIndex;
};

class RPCConection
{
	friend class RPCClient;
public:
	RPCConection();
	~RPCConection();
	static void OnAcceptPoolClinet(SocketPoolClinet* c,void *user_data);
public:

	bool Connect(int addrc, char **addrs);
	void Update(float t);
	void AddRequest(RPCRequestPack<google::protobuf::Message, google::protobuf::Message> *info);
	//void Call(ushort sig, void *send_proto, void *recv_proto, void **user_data);
	//void Call(ushort sig, void *send_proto, void *recv_proto, void **user_data,int data_len);
	//void Call(int sig,Proto::Header *m_Header, google::protobuf::Message* m_Proto);

private:
	void AddLostAddr(int fd);
	RPCRequestPack<google::protobuf::Message, google::protobuf::Message>* GetRequest();
	
public:
	ObjectPool<RPCClient> m_CachedClient;
	
	ConnectResult *m_AllInfoServerAddr;
	int m_AllInfoServerLength;
	std::queue<RPCRequestPack<google::protobuf::Message, google::protobuf::Message>*> m_RequestQueue;
	Core::MutexLock m_Lock;
	Timer m_UpdateTimer;
	float m_ReconectLostAddrTime;
	SocketPool m_SocketPool;
	Core::Event *m_Event;
};

extern RPCRequestSig RPC_ALL_REQUEST_INFO[RPC_SIG_COUNT];

template<class T1, class T2>
inline void RpcCall(RPCConection *con,int sig, Proto::Protocol::Header &header,T1 &request , RPCInnerArg* arg)
{
	RPCRequestPack<T1, T2> *pack = CreateInnerRpcRequsetPack<T1,T2>();
	pack->m_SIG = sig;
	if(arg)pack->m_InnerArg = *arg;
	pack->m_RequestHeader.CopyFrom(header);
	if(pack->m_SIG != RPC_SIG_CHANNEL_INNER_PROXY && pack->m_SIG != RPC_SIG_CLIENT_PROXY)
	{
		pack->m_RequestHeader.set_methodname(RPC_ALL_REQUEST_INFO[pack->m_SIG].m_MethodName);
	}
	pack->m_RequestHeader.set_type(Proto::Protocol::PacketType::REQUEST);
	pack->m_RequestHeader.set_rpcid(pack->m_SIG);
	pack->m_Request->CopyFrom(request);
	con->AddRequest(reinterpret_cast<RPCRequestPack<google::protobuf::Message, google::protobuf::Message>*>(pack));
	//con->m_Lock.Lock();
	//con->m_RequestQueue.push();
	//con->m_Lock.Unlock();
};

inline void RpcProxyCall(RPCConection *con,int sig, Proto::Protocol::Header &header, void* request, int request_length, RPCInnerArg* arg)
{
	RPCRequestPack<google::protobuf::Message, google::protobuf::Message> *pack = CreateProxyRpcRequsetPack(request, request_length);
	if (pack == NULL)return;
	pack->m_SIG = sig;
	if (arg)pack->m_InnerArg = *arg;
	pack->m_RequestHeader.CopyFrom(header);
	pack->m_RequestHeader.set_type(Proto::Protocol::PacketType::REQUEST);
	pack->m_RequestHeader.set_rpcid(pack->m_SIG);
	con->AddRequest(pack);

}

#endif