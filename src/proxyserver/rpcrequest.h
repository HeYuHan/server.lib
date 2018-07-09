#pragma once
#ifndef __RPC_REQUEST_H__
#define __RPC_REQUEST_H__
#define MAX_RPC_USER_DATA_LEN 4

#include "RpcProtocol.pb.h"




template<class T1,class T2>
class RPCRequestPack
{
public:
	Proto::Protocol::Header m_RequestHeader;
	Proto::Protocol::Header m_ResponseHeader;
	void* m_RequsetProxy;
	void* m_ResponseProxy;
	int m_RequestProxyLength;
	int m_ResponseProxyLength;
	T1* m_Request;
	T2* m_Response;
	int m_SIG;
	void *m_UserData[MAX_RPC_USER_DATA_LEN];
	int m_UserDataLen;
	bool m_Send;
	bool m_Processed;
	bool m_IsProxy;

	RPCRequestPack():m_RequsetProxy(NULL),
		m_ResponseProxy(NULL),
		m_RequestProxyLength(0),
		m_ResponseProxyLength(0),
		m_Request(NULL),
		m_Response(NULL),
		m_UserDataLen(0),
		m_Send(false),
		m_Processed(false),
		m_IsProxy(false)
	{
		//m_Request = new T1();
		//m_Response = new T2();
	}
	~RPCRequestPack()
	{
		if (m_Request)
		{
			delete m_Request;
			m_Request = NULL;
		}
		if (m_Response)
		{
			delete m_Response;
			m_Response = NULL;
		}
		
		

		if (m_RequsetProxy)
		{
			delete[] m_RequsetProxy;
			m_RequsetProxy = NULL;
			m_RequestProxyLength = 0;
		}
		//if (m_ResponseProxy)
		//{
		//	delete[] m_ResponseProxy;
		//	m_ResponseProxy = NULL;
		//	m_ResponseProxyLength = 0;
		//}
		m_ResponseProxyLength = 0;
		m_IsProxy = false;
		m_UserDataLen = 0;
	}
};

template<class T1, class T2>
inline RPCRequestPack<T1, T2> * CreateInnerRpcRequsetPack()
{
	RPCRequestPack<T1, T2> *pack = new RPCRequestPack<T1, T2>();
	pack->m_IsProxy = false;
	pack->m_Request = new T1();
	pack->m_Response = new T2();
	return pack;
}

inline RPCRequestPack<google::protobuf::Message, google::protobuf::Message> * CreateProxyRpcRequsetPack(const void* request_data,int data_length)
{
	if (request_data == NULL || data_length == 0)return NULL;
	RPCRequestPack<google::protobuf::Message, google::protobuf::Message> *pack = new RPCRequestPack<google::protobuf::Message, google::protobuf::Message>();
	pack->m_IsProxy = true;
	pack->m_RequsetProxy = new char[data_length];
	memcpy(pack->m_RequsetProxy, request_data, data_length);
	pack->m_RequestProxyLength = data_length;
	return pack;
}



typedef void(*RPCCallbak)(RPCRequestPack<google::protobuf::Message, google::protobuf::Message>*);
struct RPCRequestSig
{
	const char* m_MethodName;
	RPCCallbak m_CallBack;
};
typedef enum
{
	RPC_SIG_NONE,
	RPC_SIG_LOGIN=1,
	RPC_SIG_CLIENT_PROXY,
	RPC_SIG_INNER_PROXY,
	RPC_SIG_COUNT,
}RPCSIG;
#endif