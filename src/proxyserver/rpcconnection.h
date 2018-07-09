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
	virtual void OnRevcMessage();
	virtual void OnDisconnect();
	virtual void OnConnected();
	virtual void OnReconnected(SocketPoolClinet*);
	virtual void OnWrite();
	void Init();
	bool IsConnect();
	

public:

	uint uid;
	RPCConection *m_RPC;
	RPCRequestPack<google::protobuf::Message, google::protobuf::Message> *m_CurrentRequest;
	//Core::MutexLock m_Lock;
private:
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
};

extern RPCRequestSig RPC_ALL_REQUEST_INFO[RPC_SIG_COUNT];

template<class T1, class T2>
inline void RpcCall(RPCConection *con, int sig, Proto::Protocol::Header &header,T1 &request ,void **user_data, int data_len)
{
	RPCRequestPack<T1, T2> *pack = CreateInnerRpcRequsetPack<T1,T2>();
	pack->m_SIG = sig;
	pack->m_RequestHeader.CopyFrom(header);
	if(pack->m_SIG != RPC_SIG_INNER_PROXY && pack->m_SIG != RPC_SIG_CLIENT_PROXY)
	{
		pack->m_RequestHeader.set_methodname(RPC_ALL_REQUEST_INFO[pack->m_SIG].m_MethodName);
	}
	pack->m_RequestHeader.set_type(Proto::Protocol::PacketType::REQUEST);
	pack->m_RequestHeader.set_rpcid(pack->m_SIG);
	pack->m_Request->CopyFrom(request);
	data_len = MIN(data_len, MAX_RPC_USER_DATA_LEN);
	pack->m_UserDataLen = data_len;
	for (int i = 0; i < data_len; i++)
	{
		pack->m_UserData[i] = user_data[i];
	}
	con->AddRequest(reinterpret_cast<RPCRequestPack<google::protobuf::Message, google::protobuf::Message>*>(pack));
	//con->m_Lock.Lock();
	//con->m_RequestQueue.push();
	//con->m_Lock.Unlock();
};

inline void RpcProxyCall(RPCConection *con, int sig, Proto::Protocol::Header &header, void* request, int request_length, void **user_data, int data_len)
{
	RPCRequestPack<google::protobuf::Message, google::protobuf::Message> *pack = CreateProxyRpcRequsetPack(request, request_length);
	if (pack == NULL)return;

	pack->m_SIG = sig;
	pack->m_RequestHeader.CopyFrom(header);
	pack->m_RequestHeader.set_type(Proto::Protocol::PacketType::REQUEST);
	pack->m_RequestHeader.set_rpcid(pack->m_SIG);
	data_len = MIN(data_len, MAX_RPC_USER_DATA_LEN);
	for (int i = 0; i < data_len; i++)
	{
		pack->m_UserData[i] = user_data[i];
	}
	con->AddRequest(pack);

}

#endif