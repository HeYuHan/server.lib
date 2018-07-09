#include "server.h"
#include <ThreadEventPool.h>
#include "ClientData.pb.h"
#include <google/protobuf/message.h>
#include "message_type.h"
#include "channelserver.h"
#include <HttpClient.h>
using namespace Proto::Protocol;
using namespace Proto::Message;
using namespace google::protobuf;
Server gServer;
Server::Server()
{

}

Server::~Server()
{


}
void OnAcceptPoolClinet(SocketPoolClinet* c, void *arg)
{
	Server* server = static_cast<Server*>(arg);
	Client * client = server->NewClient();
	if (!client)
	{

	return;
	}
	client->Init();
	client->uid = c->uid;
	c->m_Type = TCP_SOCKET;
	client->connection = c;
	c->m_Handle = client;

}


bool Server::Init()
{
	BaseServer::Init();
	Timer::GetEventBase();

	gEventPool.Init(10);
	int max_client = 10;
	m_CachedClients.Initialize(max_client);
	gSocketPool.Init(10, true, OnAcceptPoolClinet, true, this);

	//create server to channel server

	int max_channel = 5;
	m_ChannelListener.Init(max_channel);
	m_ChannelListener.CreateTcpServer("0.0.0.0:9201",max_channel);


	CreateTcpServer("0.0.0.0:9200", max_client);

	SetRpcRequest();






	char *addrs[2] = {
		/*"192.168.1.15:8000",
		"127.0.0.1:8000",*/
		"192.168.253.28:8000"

	};
	m_RPCService.Connect(1, addrs);
	

	return true;
}
int Server::Run()
{
	Init();
	int ret = BaseServer::Run();
	return ret;
}



void Server::OnTcpAccept(evutil_socket_t socket, sockaddr *addr)
{
	gSocketPool.AcceptClient(socket, addr);
}

void Server::AddOnlineClient(Client * c)
{
	Core::EasyMutexLock l(m_Lock);
	if (c->m_IsOnline)return;
	c->m_IsOnline = true;
	m_OnLineClients.push_back(c);
}

void Server::RemoveOnlineClient(Client * c)
{
	Core::EasyMutexLock l(m_Lock);
	NS_VECTOR::vector<Client*>::iterator iter = std::find(m_OnLineClients.begin(), m_OnLineClients.end(), c);
	if (iter != m_OnLineClients.end())
	{
		c->m_IsOnline = false;
		m_OnLineClients.erase(iter);
	}
	
}

Client* Server::GetClient(unsigned int uid)
{
	return m_CachedClients.Get(uid);
}

Client * Server::NewClient()
{
	return m_CachedClients.Allocate();
}

void Server::FreeClient(unsigned int uid)
{
	m_CachedClients.Free(uid);
}
void Server::ClientRpcProxy(Client* c, const char* methodName, int sig, void *request, int request_length)
{
	if (!c->IsLogin())
	{
		c->BeginWrite();
		c->WriteByte(SM_RPC_RESOPONSE);
		c->WriteInt(sig);
		c->WriteByte(ERROR_NOT_LOGIN);
		c->EndWrite();
		return;
	}
	Header header;
	char name[128];
	sprintf(name, "c_%s", methodName);
	header.set_methodname(name);
	header.set_uid(c->m_AccountId);
	header.set_pid(c->m_PlayerId);
	
	void *user_data[2] = { c,&sig };
	RPC_PROXY_CALL(RPC_SIG_CLIENT_PROXY, header, request, request_length, user_data, 2);
}
void Server::Login(Client * c)
{
	Proto::Protocol::Header header;
	Proto::Message::UserLoginRequest request;
	request.set_username(c->m_Name);
	void* user_data[1] = { c };
	RPC_CALL(UserLogin, RPC_SIG_LOGIN, header, request, user_data, 1);
}
//rpc set
BEGIN_RPC_CALLBACK(UserLogin)
{
	log_info("rpc response:%s", response->m_RequestHeader.methodname().c_str());
	if (response->m_UserDataLen == 0)return;
	Client* client = static_cast<Client*>(response->m_UserData[0]);
	int error_code = response->m_ResponseHeader.code();
	client->BeginWrite();
	client->WriteByte(SM_LOGIN);
	client->WriteInt(error_code);
	if (error_code == 0)
	{
		client->m_AccountId = response->m_Response->id();
		client->WriteProtoBuffer((google::protobuf::Message*)response->m_Response);
	}

	client->EndWrite();
}
END_RPC_CALLBACK()

BEGIN_PROXY_RPC_CALLBACK(ClientProxy)
{
	log_info("rpc response:%s", response->m_RequestHeader.methodname().c_str());
	Client* client = static_cast<Client*>(response->m_UserData[0]);
	int sig = *((int*)response->m_UserData[1]);
	int error_code = response->m_ResponseHeader.code();
	if (error_code>0)
	{
		client->BeginWrite();
		client->WriteByte(SM_RPC_RESOPONSE);
		client->WriteShort(sig);
		client->WriteShort(error_code);
		client->EndWrite();
	}
	else
	{

		client->BeginWrite();
		client->WriteByte(SM_RPC_RESOPONSE);
		client->WriteShort(sig);
		client->WriteShort(0);
		client->WriteData(response->m_ResponseProxy, response->m_ResponseProxyLength);
		client->EndWrite();
	}
}
END_RPC_CALLBACK()

BEGIN_PROXY_RPC_CALLBACK(InnerProxy)
{
	ChannelClient* client = static_cast<ChannelClient*>(response->m_UserData[0]);
	int sig = *((int*)response->m_UserData[1]);
	client->BeginWrite();
	client->WriteByte(ChannelInner::SM_CHANNEL_RPC);
	//write header
	{
		response->m_ResponseHeader.set_rpcid(sig);
		int header_size = response->m_ResponseHeader.ByteSize();
		client->WriteInt(header_size);
		int empty_size = client->write_buff_end - client->write_end;
		if (empty_size < header_size)
		{
			log_error("buffer size too small header size:%d", header_size);
		}
		else
		{
			response->m_ResponseHeader.SerializeToArray(client->write_end, empty_size);
			client->write_end += header_size;
		}
	}
	//write msg
	{
		client->WriteData(response->m_ResponseProxy, response->m_RequestProxyLength);
	}
	client->EndWrite();
}
END_RPC_CALLBACK()


void Server::SetRpcRequest()
{
	RPC_ALL_REQUEST_INFO[RPC_SIG_LOGIN].m_MethodName = "c_UserLogin";
	RPC_ALL_REQUEST_INFO[RPC_SIG_LOGIN].m_CallBack = Rpc_UserLogin_CallBack;
	//proxy
	RPC_ALL_REQUEST_INFO[RPC_SIG_CLIENT_PROXY].m_CallBack = Rpc_ClientProxy_CallBack;
	RPC_ALL_REQUEST_INFO[RPC_SIG_INNER_PROXY].m_CallBack = Rpc_InnerProxy_CallBack;
}
