#include "channelserver.h"
#include "server.h"
#include<log.h>

using namespace ChannelInner;
ChannelListener::ChannelListener()
{
}

ChannelListener::~ChannelListener()
{
}

void ChannelListener::OnTcpAccept(evutil_socket_t socket, sockaddr *addr)
{
	m_SocketPool.AcceptClient(socket, addr);
}

bool ChannelListener::Init(int channel_count)
{
	m_SocketPool.Init(channel_count, false, ChannelListener::OnAcceptPoolClinet, true, this);
	m_CachedClient.Initialize(channel_count);
	return true;
}

void ChannelListener::OnAcceptPoolClinet(NS_CORE::SocketPoolClinet * c, void * arg)
{
	ChannelListener* listener = static_cast<ChannelListener*>(arg);
	ChannelClient * client = listener->m_CachedClient.Allocate();
	if (!client)
	{

		return;
	}
	client->Init();
	client->m_Listener = listener;
	client->uid = c->uid;
	c->m_Type = TCP_SOCKET;
	client->connection = c;
	c->m_Handle = client;
}

ChannelClient::ChannelClient():m_Listener(NULL)
{
}

ChannelClient::~ChannelClient()
{
}

void ChannelClient::OnMessage()
{
	NS_CORE::byte cmd = 0;
	ReadByte(cmd);
	switch (cmd)
	{
	case CM_CHANNEL_INFO:
	{
		SetInfo();
		break;
	}
	case CM_CHANNEL_RPC:
	{
		RpcRequest();
		break;
	}
	default:
		log_error("cant parse msg : %d", cmd);
		this->connection->Disconnect();
		break;
	}
}

bool ChannelClient::ThreadSafe()
{
	return true;
}

uint ChannelClient::GetUid()
{
	return uint();
}

void ChannelClient::OnWrite()
{
}

void ChannelClient::OnRevcMessage()
{
	NetworkStream::OnRevcMessage();
}

void ChannelClient::OnDisconnect()
{
	if (!m_Listener)m_Listener->m_CachedClient.Free(uid);
	log_info("channel client disconnected inner addr %s", m_InnerAddr);
}

void ChannelClient::OnConnected()
{
	SocketPoolClinet *con = static_cast<SocketPoolClinet*>(this->connection);
	sprintf(m_InnerAddr, "%s:%d", inet_ntoa(con->m_SockAddr.sin_addr), ntohs(con->m_SockAddr.sin_port));
	log_info("new channel client connected inner addr %s", m_InnerAddr);
}

void ChannelClient::OnReconnected(SocketPoolClinet *)
{
}

void ChannelClient::Init()
{
	m_Listener = NULL;
	memset(m_InnerAddr, 0, sizeof(m_InnerAddr));
	memset(m_OuterAddr, 0, sizeof(m_OuterAddr));
}

void ChannelClient::SetInfo()
{
	ReadString(m_OuterAddr, sizeof(m_OuterAddr));
	log_info("new channel outer addr:%s", m_OuterAddr);
}

void ChannelClient::SyncState()
{
}

void ChannelClient::RpcRequest()
{
	
	int header_len = 0;

	ReadInt(header_len);
	if (header_len == 0)
	{
		log_error("parse channel head length error len:%d", 0);
		return;
	}

	Proto::Protocol::Header header;
	if (!header.ParseFromArray(read_position, header_len))
	{
		log_error("cant parse channel rpc header len:%d", header_len);
		return;
	}
	read_position += header_len;
	int sig = header.rpcid();
	void* user_data[2] = { this,&sig};
	RPC_PROXY_CALL(RPC_SIG_INNER_PROXY, header, read_position, read_end - read_position, user_data, 2);
}
