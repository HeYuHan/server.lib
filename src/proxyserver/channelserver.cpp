#include "channelserver.h"
#include "server.h"
#include<log.h>
#include "room.h"
#include <ProxyMessageProtocol.pb.h>
#include <InnerMessageProtocol.pb.h>
#include <AllMessagePackages.pb.h>
#include <CommonTypes.pb.h>
#ifndef WIN32
#include <arpa/inet.h>
#endif // !WIN32
using namespace Proto::Protocol;
using namespace Proto::Types;
using namespace Proto::Protocol::Inner;
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

ChannelClient * ChannelListener::GetCanUseChannelRoom()
{
	ChannelClient *result = m_CachedClient.Begin();
	int rate = 100;
	for (size_t i = 0; i < m_CachedClient.Size(); i++)
	{
		ChannelClient *client = m_CachedClient.Begin() + i;
		if (client->IsValid())
		{
			int r = client->GetRate();
			if (r < rate)
			{
				rate = r;
				result = client;
			}
		}
	}
	if (rate < 100)return result->IsValid()?result:NULL;
	return NULL;
}

void ChannelListener::FreeRoom(uint channel, uint room)
{
	ChannelClient *channelClient = m_CachedClient.Get(channel);
	if (channelClient && channelClient->IsValid())
	{
		channelClient->FreeRoom(room);
	}
	else
	{
		log_error("cant free channel room cid:%d,rid:%d", channel, room);
	}
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
	case CM_CHANNEL_STATE:
	{
		SyncState();
		break;
	}
	case CM_CHANNEL_START_GAME:
	{
		OnStartGame();
		break;
	}
	case CM_CHANNEL_VALIDATE_ENTER_ROOM:
	{
		ValidateEnterRoom();
		break;
	}
	case CM_CHANNEL_LEAVE_ROOM:
	{
		LeaveRoom();
		break;
	}
	case CM_CHANNEL_CHANGE_ROOM_STATE:
	{
		OnChangeRoomState();
		break;
	}
	case CM_CHANNEL_ERROR:
	{
		Core::byte error;
		ReadByte(error);
		log_error("channel error:%d", error);
		break;
	}
	default:
		log_error("cant parse msg : %d", cmd);
		if(this->connection)this->connection->Disconnect();
		break;
	}
}

bool ChannelClient::ThreadSafe()
{
	return true;
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
	Room* r = gServer.m_CachedRooms.Begin();
	for (size_t i = 0; i < gServer.m_CachedRooms.Size(); i++)
	{
		Room* temp = r + i;
		if (temp && !temp->m_Free && temp->m_Channel&&temp->m_Channel->uid == uid)
		{
			gServer.FreeRoom(temp);
		}
	}
	if (m_Listener)m_Listener->m_CachedClient.Free(uid);
	
	log_info("channel client disconnected inner addr %s", m_InnerAddr);
	this->connection = NULL;
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

bool ChannelClient::IsValid()
{
	return this->connection!=NULL && this->m_Listener != NULL;
}

void ChannelClient::Init()
{
	m_Listener = NULL;
	m_RoomCount = 0;
	NetworkStream::Reset();
	memset(m_InnerAddr, 0, sizeof(m_InnerAddr));
	memset(m_OuterAddr, 0, sizeof(m_OuterAddr));
}

void ChannelClient::SetInfo()
{
	ReadString(m_OuterAddr, sizeof(m_OuterAddr));
	ReadInt(m_RoomMaxCount);
	log_info("new channel outer addr:%s", m_OuterAddr);
}

void ChannelClient::SyncState()
{
	ReadInt(m_RoomCount);
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
	RPCInnerArg arg;
	arg.m_ProxySig = sig;
	arg.m_UID = this->uid;
	RPC_PROXY_CALL(RPC_SIG_INNER_PROXY, header, read_position, read_end - read_position, &arg);
}

int ChannelClient::GetRate()
{
	return (m_RoomCount * 100) / m_RoomMaxCount;
}

void ChannelClient::OnStartGame()
{
	uint client_id = 0;
	uint proxy_room_uid = 0;
	uint channel_room_uid = 0;
	ReadUInt(client_id);
	ReadUInt(proxy_room_uid);
	ReadUInt(channel_room_uid);
	Client* client = gServer.m_CachedClients.Get(client_id);
	if (!client || client->m_State != CS_IN_ROOM)
	{
		log_error("start game room host not found:%d or state error:%d", client_id,client->m_State);
		FreeRoom(channel_room_uid);
		return;
	}
	
	Room* room = gServer.m_CachedRooms.Get(proxy_room_uid);
	if (!room)
	{
		log_error("start game proxy room not found %d",proxy_room_uid);
		FreeRoom(channel_room_uid);
		return;
	}
	if (channel_room_uid == 0)
	{
		client->ResponseError(Proxy::ERROR_CHANNEL_FULL);
		room->m_RoomState = RS_LOAD;
		return;
	}
	log_info("channel room create uid:%d", channel_room_uid);

	room->Lock();
	room->SetLongId(uid, channel_room_uid);
	room->m_Channel = this;
	room->UnLock();
	room->EachClient([=](Client* client) {
		client->BeginWrite();
		client->WriteByte(Proxy::SM_START_GAME);
		client->WriteString(this->m_OuterAddr);
		client->WriteULong(room->GetToken());
		client->EndWrite();
	});
}
void ChannelClient::OnChangeRoomState()
{
	uint proxy_room_uid = 0;
	uint channel_room_uid = 0;
	Core::byte state = 0;
	ReadUInt(proxy_room_uid);
	ReadUInt(channel_room_uid);
	ReadByte(state);
	Room * r = gServer.m_CachedRooms.Get(proxy_room_uid);
	if (!r)return;
	r->m_RoomState = state;
	
	if (state == RS_RELEASE)
	{
		
		r->Lock();
		if (r&&r->GetRoomId() == channel_room_uid)
		{
			r->m_Channel = NULL;
			gServer.FreeRoom(r);
		}
		else
		{
			log_error("not find room %d", r->uid);
		}
		r->UnLock();
	}
}
//��֤channel���뷿��
void ChannelClient::ValidateEnterRoom()
{
	uint proxy_room_id=0;
	uint proxy_client_id=0;
	uint channel_client_uid = 0;
	ReadUInt(proxy_room_id);
	ReadUInt(proxy_client_id);
	ReadUInt(channel_client_uid);
	Room *r = gServer.m_CachedRooms.Get(proxy_room_id);
	Client *client = gServer.GetClient(proxy_client_id);
	bool validate = client && r && r->HavePlayer(proxy_client_id);
	if (validate)client->m_State = CS_IN_PLAY_GAME;
	BeginWrite();
	WriteByte(SM_CHANNEL_VALIDATE_ENTER_ROOM);
	WriteByte(validate ? 0 : ERROR_ROOM_OR_PLAYER_NOT_FOUND);
	WriteUInt(channel_client_uid);
	if(validate)WriteProtoBuffer((google::protobuf::Message*)&client->m_ClientInfo);
	EndWrite();
}

void ChannelClient::LeaveRoom()
{
	uint proxy_client_id = 0;
	ReadUInt(proxy_client_id);
	Client *client = gServer.GetClient(proxy_client_id);
	if (client && client->m_OwnerRoom)
	{
		if (!client->m_OwnerRoom->m_Free &&client->m_OwnerRoom->RemovePlayer(client))
		{
			client->m_State = CS_IN_LOBBY;
		}
		log_info("channel client leave room uid:%d", client->uid);
	}
}


void ChannelClient::FreeRoom(uint roomid)
{
	BeginWrite();
	WriteByte(SM_CHANNEL_RELEASE_ROOM);
	WriteUInt(roomid);
	EndWrite();
}
