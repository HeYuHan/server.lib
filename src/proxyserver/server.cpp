#include "server.h"
#include <ThreadEventPool.h>
#include "ClientData.pb.h"
#include <ProxyMessageProtocol.pb.h>
#include <InnerMessageProtocol.pb.h>
#include <CommonTypes.pb.h>
#include "channelserver.h"
#include <HttpClient.h>
#include <algorithm>
#include <LuaEngine.h>
using namespace Proto::Protocol;
using namespace Proto::Protocol::Proxy;
using namespace Proto::Types;
using namespace Proto::Message;
using namespace google::protobuf;

class ProxyConfig:public LuaInterface
{
	GET_SET(int, MaxRoom)
	GET_SET(int, MaxChannel)
	GET_SET(int, EeventPoolSize)
	GET_SET(int, MaxClient)
public:
	

private:
	virtual const char* GetRequireScript() { return "proxyserver/config"; };
	virtual const char* GetNativeTypeName() { return "ProxyConfig"; };
public:
	bool SetByLua()
	{
		bool ok = LuaEngine::GetInstance()->CreateScriptHandle(this);
		if (ok)
		{
			this->EnterScript(this);
			return true;
		}
		return false;
	}
}gServerConfig;





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

int reg_lua(lua_State* L)
{
	BEGIN_BASE_CLASS(ProxyConfig)
		REG_GET_SET(MaxRoom)
		REG_GET_SET(MaxChannel)
		REG_GET_SET(EeventPoolSize)
		REG_GET_SET(MaxClient)
	END_CLASS()
	return 0;
}





bool Server::Init()
{
	if (!BaseServer::Init())return false;
	Timer::GetEventBase();
	LuaEngine::GetInstance()->RegisterModel(reg_lua);
	LuaEngine::GetInstance()->Start();
	LuaEngine::GetInstance()->LuaSearchPath("path", "./../src/script/?.lua");
	LuaEngine::GetInstance()->LuaSearchPath("path", "./../script/?.lua");
	LuaEngine::GetInstance()->LuaSearchPath("path", "./script/?.lua");

	do
	{
		if (!gServerConfig.SetByLua())
		{
			log_error("%s", "cant init game config");
			break;
		}

		if (!m_CachedClients.Initialize(gServerConfig.GetMaxClient()))
		{
			log_error("%s size:%d", "init client pool failed", gServerConfig.GetMaxClient());
			break;
		}
		if (!m_CachedRooms.Initialize(gServerConfig.GetMaxRoom()))
		{
			log_error("%s size:%d", "init room pool failed", gServerConfig.GetMaxRoom());
			break;
		}
		if (!m_ChannelListener.Init(gServerConfig.GetMaxChannel()))
		{
			log_error("%s size:%d", "init channel pool failed", gServerConfig.GetMaxChannel());
			break;
		}
		if (!gEventPool.Init(gServerConfig.GetEeventPoolSize()))
		{
			log_error("%s size:%d", "init event pool failed", gServerConfig.GetEeventPoolSize());
			break;
		}
		if (!CreateTcpServer(m_Addr, gServerConfig.GetMaxClient()))
		{
			log_error("cant create proxy server addr:%s", m_Addr);
			break;
		}
		if (!m_ChannelListener.CreateTcpServer(m_ChannelAddr, gServerConfig.GetMaxChannel()))
		{
			log_error("cant create channel server addr:%s", m_ChannelAddr);
			break;
		}
		if (!gSocketPool.Init(gServerConfig.GetMaxClient(), true, OnAcceptPoolClinet, true, this))
		{
			log_error("%s", "init socket pool failed");
			break;
		}
		if (m_InfoAddrsLength == 0)
		{
			log_error("%s", "not have info server addrs");
			break;
		}
		SetRpcRequest();
		m_RPCService.Connect(m_InfoAddrsLength, m_InfoAddrs);
		return true;
	} while (false);
	gEventPool.Destory();
	CloseServer();
	m_ChannelListener.CloseServer();
	Timer::ExitLoop();
	return false;
}
int Server::Run()
{
	if (!Init())
	{
		return -1;
	}
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
	RPCInnerArg arg;
	arg.m_ProxySig = sig;
	arg.m_UID = c->uid;
	RPC_PROXY_CALL(RPC_SIG_CLIENT_PROXY, header, request, request_length, &arg);
}
void Server::UserLogin(Client * c)
{
	Proto::Protocol::Header header;
	Proto::Message::UserLoginRequest request;
	request.set_username(c->m_Name);
	RPCInnerArg arg;
	arg.m_UID = c->uid;
	RPC_CALL(UserLogin,RPC_SIG_USER_LOGIN, header, request, &arg);
}

void Server::PlayerLogin(Client * c)
{
	Proto::Protocol::Header header;
	header.set_uid(c->m_AccountId);
	header.set_pid(c->m_PlayerId);
	Proto::Message::PlayerLoginRequest request;
	request.set_clientversion("1.1.1.1");
	request.set_appstore("ios");
	request.set_device("unknow");
	RPCInnerArg arg;
	arg.m_UID = c->uid;
	RPC_CALL(PlayerLogin, RPC_SIG_PLAYER_LOGIN, header, request, &arg);
}

void Server::BroadCastRoomChange(Room *room, Core::byte type)
{
	for (int i = 0; i < m_CachedClients.Size(); i++)
	{
		Client *client = m_CachedClients.Begin() + i;
		if (client->IsOnline())
		{
			client->BeginWrite();
			client->WriteByte(SM_ROOM_CHANGE);
			client->WriteByte(type);
			client->WriteUInt(room->uid);
			client->EndWrite();
		}
	}
}

void Server::WriteRoomList(Client * client)
{

}

void Server::FreeRoom(Room * room)
{
	if(room->m_Channel)m_ChannelListener.FreeRoom(room->GetChannelId(), room->GetRoomId());
	BroadCastRoomChange(room, 1 << ROOM_CHANGE_REOMVE);
	room->Free();
}

void Server::ChatClient(Client *c, const char*methodName, int sig, void *request, int request_length)
{
	Header header;
	char name[128];
	sprintf(name, "c_%s", methodName);
	header.set_uid(c->m_AccountId);
	header.set_pid(c->m_PlayerId);
	RPCInnerArg arg;
	arg.m_UID = c->uid;
	
	RPC_PROXY_CALL(RPC_SIG_CHAT_INNER_PROXY, header, request, request_length, &arg);
}

//rpc set
BEGIN_RPC_CALLBACK(UserLogin)
{
	//log_info("rpc response:%s", response->m_RequestHeader.methodname().c_str());
	Client* client = gServer.m_CachedClients.Get(response->m_InnerArg.m_UID);
	if (client == NULL)
	{
		log_error("client not found uid:%d", response->m_InnerArg.m_UID);
		return;
	};
	int error_code = response->m_ResponseHeader.code();
	client->BeginWrite();
	client->WriteByte(SM_USER_LOGIN);
	client->WriteInt(error_code);
	if (error_code == 0)
	{
		client->m_AccountId = response->m_Response->id();
		client->WriteProtoBuffer((google::protobuf::Message*)response->m_Response);
	}
	client->EndWrite();
}
END_RPC_CALLBACK()

BEGIN_RPC_CALLBACK(PlayerLogin)
{
	//log_info("rpc response:%s", response->m_RequestHeader.methodname().c_str());
	Client* client = gServer.m_CachedClients.Get(response->m_InnerArg.m_UID);
	if (client == NULL)
	{
		log_error("client not found uid:%d", response->m_InnerArg.m_UID);
		return;
	};
	int error_code = response->m_ResponseHeader.code();
	client->BeginWrite();
	client->WriteByte(SM_PLAYER_LOGIN);
	client->WriteInt(error_code);
	if (error_code == 0)
	{
		client->m_IsOnline = true;
		client->m_ClientInfo.CopyFrom(response->m_Response->playerinfo());
		client->WriteProtoBuffer((google::protobuf::Message*)response->m_Response);
	}
	client->EndWrite();
}
END_RPC_CALLBACK()

BEGIN_PROXY_RPC_CALLBACK(ClientProxy)
{
	//log_info("rpc proxy response:%s", response->m_RequestHeader.methodname().c_str());
	Client* client = gServer.m_CachedClients.Get(response->m_InnerArg.m_UID);
	if (client == NULL)
	{
		log_error("client not found uid:%d", response->m_InnerArg.m_UID);
		return;
	};
	short sig = response->m_InnerArg.m_ProxySig;
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
	//log_info("rpc inner proxy response:%s", response->m_RequestHeader.methodname().c_str());
	ChannelClient* client = gServer.m_ChannelListener.m_CachedClient.Get(response->m_InnerArg.m_UID);
	if (client == NULL)
	{
		log_error("client not found uid:%d", response->m_InnerArg.m_UID);
		return;
	};
	short sig = response->m_InnerArg.m_ProxySig;
	client->BeginWrite();
	client->WriteByte(Inner::SM_CHANNEL_RPC);
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
		client->WriteData(response->m_ResponseProxy, response->m_ResponseProxyLength);
	}
	client->EndWrite();
}
END_RPC_CALLBACK()

BEGIN_PROXY_RPC_CALLBACK(ChatProxy)
{

	Chatclient* client = gServer.m_ChatListener.m_ChatCachedClient.Get(response->m_InnerArg.m_UID);
	if (client == NULL)
	{
		log_error("client not found uid:%d",response->m_InnerArg.m_UID);
		return;
	}
	short sig = response->m_InnerArg.m_ProxySig;
	client->BeginWrite();
	client->WriteByte(Inner::SM_CHAT_RPC);
	client->WriteData(response->m_ResponseProxy, response->m_ResponseProxyLength);
	client->EndWrite();

	
}
END_RPC_CALLBACK();

void Server::SetRpcRequest()
{
	RPC_ALL_REQUEST_INFO[RPC_SIG_USER_LOGIN].m_MethodName = "c_UserLogin";
	RPC_ALL_REQUEST_INFO[RPC_SIG_USER_LOGIN].m_CallBack = Rpc_UserLogin_CallBack;

	RPC_ALL_REQUEST_INFO[RPC_SIG_PLAYER_LOGIN].m_MethodName = "c_PlayerLogin";
	RPC_ALL_REQUEST_INFO[RPC_SIG_PLAYER_LOGIN].m_CallBack = Rpc_PlayerLogin_CallBack;

	//proxy
	RPC_ALL_REQUEST_INFO[RPC_SIG_CLIENT_PROXY].m_CallBack = Rpc_ClientProxy_CallBack;
	RPC_ALL_REQUEST_INFO[RPC_SIG_INNER_PROXY].m_CallBack = Rpc_InnerProxy_CallBack;
	RPC_ALL_REQUEST_INFO[RPC_SIG_CHAT_INNER_PROXY].m_CallBack = Rpc_ChatProxy_CallBack;


}
