#include "client.h"
#include "server.h"
#include <ProxyMessageProtocol.pb.h>
#include <InnerMessageProtocol.pb.h>
#include <CommonTypes.pb.h>
#include "ClientData.pb.h"
#include "room.h"
using namespace Proto::Protocol::Proxy;
using namespace Proto::Types;
Client::Client():m_IsOnline(false),m_AccountId(0),m_PlayerId(0),m_OwnerRoom(NULL)
{

}
void Client::Init()
{
	m_IsOnline = false;
	m_AccountId = 0;
	m_PlayerId = 0;
	m_OwnerRoom = NULL;
	m_State = CS_NONE;
}
void Client::WriteClientInfo(NetworkStream * stream)
{
	stream->WriteProtoBufferAutoSize((google::protobuf::Message*)&m_ClientInfo);
	stream->WriteProtoBufferAutoSize((google::protobuf::Message*)&m_Equip);
}
void Client::RequestRpc()
{
	char methodName[65] = {0};
	
	short sig = 0;
	ReadString(methodName, 65);
	ReadShort(sig);
	int size = read_end - read_position;

	/*Proto::Message::ProtoPlayer p;
	bool b = p.ParseFromArray(read_position, size);
	p.set_name("1234567");*/


	/*BeginWrite();
	WriteByte(SM_RPC_RESOPONSE);
	WriteShort(sig);
	WriteShort(0);
	
	int s = p.ByteSize();
	p.SerializeToArray(write_end, write_buff_end-write_buff);
	write_end += s;

	EndWrite();*/

	gServer.ClientRpcProxy(this, methodName, sig, read_position, size);
}
void Client::RequestCreateRoom()
{
	if (this->m_OwnerRoom != NULL || this->m_State == CS_IN_ROOM)
	{
		log_error("client is state error state %d", this->m_State);
		ResponseError(ERROR_ROOM_FULL);
		return;
	}
	
	
	Room* room = gServer.m_CachedRooms.Allocate();
	if (!room)
	{
		log_error("proxy cant allocate more room cache size:%d", gServer.m_CachedRooms.Size());
		ResponseError(ERROR_ROOM_FULL);
		return;
	}
	bool ok = ReadProtoBufferAutoSize((google::protobuf::Message*)&m_Equip);
	ok = ok && ReadProtoBufferAutoSize((google::protobuf::Message*)&room->m_RoomCreateOption) && room->m_RoomCreateOption.name().size();
	if (!ok)
	{
		log_error("proxy cant parse room create option %d", room->uid);
		ResponseError(ERROR_ROOM_FULL);
		return;
	}
	log_info("proxy room create uid:%d", room->uid);
	room->Lock();
	room->Init();
	room->AddPlayer(this);
	room->m_HostId = uid;
	room->UnLock();
	this->m_State = CS_IN_ROOM;
	BeginWrite();
	WriteByte(SM_CREATE_ROOM);
	WriteUInt(room->uid);
	EndWrite();
	gServer.BroadCastRoomChange(room, 1 << ROOM_CHANGE_CREATE);
	
	
	/*if (!ok)
	{
		ResponseError(ERROR_ROOM_FULL);
		return;
	}
	
	ChannelClient * channel = gServer.m_ChannelListener.GetCanUseChannelRoom();
	if (channel)
	{
		this->m_State = CS_CREATE_ROOM;
		channel->BeginWrite();
		channel->WriteByte(Proto::Protocol::Inner::SM_CHANNEL_CREATE_ROOM);
		channel->WriteUInt(uid);
		channel->EndWrite();
	}
	else
	{
		ResponseError(ERROR_ROOM_FULL);
	}*/
}
void Client::RequestRoomList()
{
	short page;
	short page_count;
	ReadShort(page);
	ReadShort(page_count);
	int all_room_count = gServer.m_CachedRooms.Count();
	int skip_count = page* page_count;
	int more_page = 0;
	int more_room = all_room_count - skip_count;
	if (more_room > page_count)
	{
		more_page = more_room / page_count;
		if (more_page * page_count < more_room)
		{
			more_page += 1;
		}
		more_page -= 1;
	}
	BeginWrite();
	WriteByte(SM_ROOM_LIST);
	//page
	WriteShort(page);
	//morepage
	WriteShort(more_page);
	char *count_pos = write_end;
	//write_count
	WriteShort(0);
	int index = 0;
	short write_count = 0;
	for (int i = 0; i < gServer.m_CachedRooms.Size(); i++)
	{
		Room * room = gServer.m_CachedRooms.Begin() + i;
		if (!room->m_Free)
		{
			
			if (index == skip_count)
			{
				write_count++;
				room->WriteSampleRoomInfo(this);
				if (write_count == page_count)break;
			}
			else
			{
				index++;
			}
		}
	}
	memcpy(count_pos, &write_count, sizeof(short));
	EndWrite();
	
}

void Client::RequestOBRoomList()
{
	short page;
	short page_count;
	ReadShort(page);
	ReadShort(page_count);
	int all_room_count = 0;
	for (int i = 0; i < gServer.m_CachedRooms.Size(); i++)
	{
		Room * room = gServer.m_CachedRooms.Begin() + i;
		if (!room->m_Free && room->m_Channel && room->m_RoomState == RS_PLAY)
		{
			all_room_count++;
			
		}
	}
	int skip_count = page* page_count;
	int more_page = 0;
	int more_room = all_room_count - skip_count;
	if (more_room > page_count)
	{
		more_page = more_room / page_count;
		if (more_page * page_count < more_room)
		{
			more_page += 1;
		}
		more_page -= 1;
	}
	BeginWrite();
	WriteByte(SM_OB_ROOM_LIST);
	//page
	WriteShort(page);
	//morepage
	WriteShort(more_page);
	char *count_pos = write_end;
	//write_count
	WriteShort(0);
	int index = 0;
	short write_count = 0;
	for (int i = 0; i < gServer.m_CachedRooms.Size(); i++)
	{
		Room * room = gServer.m_CachedRooms.Begin() + i;
		if (!room->m_Free && room->m_Channel && room->m_RoomState == RS_PLAY)
		{

			if (index == skip_count)
			{
				write_count++;
				room->WriteSampleObRoomInfo(this);
				if (write_count == page_count)break;
			}
			else
			{
				index++;
			}
		}
	}
	memcpy(count_pos, &write_count, sizeof(short));
	EndWrite();
}

void Client::RequestRoomInfo()
{
	uint roomuid = 0;
	ReadUInt(roomuid);
	BeginWrite();
	WriteByte(SM_ROOM_INFO);
	Room *r = gServer.m_CachedRooms.Get(roomuid);
	if (r)
	{
		r->WriteDetailRoomInfo(this);
	}
	EndWrite();
}

void Client::RequestEnterRoom()
{
	if (this->m_State == CS_IN_ROOM)return;
	ReadProtoBufferAutoSize((google::protobuf::Message*)&m_Equip);

	uint room_uid = 0;
	ReadUInt(room_uid);
	Room *r = gServer.m_CachedRooms.Get(room_uid);
	if (r)
	{
		r->Lock();
		if (!r->m_Free && r->AddPlayer(this))
		{
			this->m_State = CS_IN_ROOM;
			BeginWrite();
			WriteByte(SM_ENTER_ROOM);
			WriteByte(r->m_RoomState);
			EndWrite();
			gServer.BroadCastRoomChange(r, OFFSET(ROOM_CHANGE_SAMPLE));
		}
		else
		{
			ResponseError(ERROR_ROOM_FULL);
		}
		r->UnLock();
	}
}

void Client::RequestLeaveRoom()
{
	Room* r = m_OwnerRoom;
	if (r)
	{
		if (r->RemovePlayer(this))
		{
			this->m_State = CS_IN_LOBBY;
			
			if(r->m_CurrentPlayerCount>0)gServer.BroadCastRoomChange(r, OFFSET(ROOM_CHANGE_SAMPLE));
		}
		else
		{
			log_error("cant remove client %d", uid);
		}
	}
}
void Client::RequestStartGame()
{
	if (!this->m_OwnerRoom || this->m_OwnerRoom->m_HostId != uid || this->m_OwnerRoom->m_RoomState != RS_WAIT)
	{
		log_error("cant start game room:%p",this);
		return;
	}
	log_info("start game proxy room uid:%d", m_OwnerRoom->uid);
	m_OwnerRoom->Lock();
	if (m_OwnerRoom->m_Free)return;

	

	ChannelClient * channel = gServer.m_ChannelListener.GetCanUseChannelRoom();
	if (channel)
	{
		this->m_OwnerRoom->m_RoomState = RS_ENTER_CHANNEL;
		channel->BeginWrite();
		channel->WriteByte(Proto::Protocol::Inner::SM_CHANNEL_START_GAME);
		channel->WriteUInt(uid);
		channel->WriteUInt(m_OwnerRoom->uid);
		channel->WriteProtoBufferAutoSize((google::protobuf::Message*)&m_OwnerRoom->m_RoomCreateOption);
		channel->WriteByte(m_OwnerRoom->m_CurrentPlayerCount);
		
		m_OwnerRoom->EachClient([&](Client* client) {
			channel->WriteUInt(client->uid);
			channel->WriteProtoBufferAutoSize((google::protobuf::Message*)&client->m_ClientInfo);
			channel->WriteProtoBufferAutoSize((google::protobuf::Message*)&client->m_Equip);
		});
		channel->EndWrite();
	}
	else
	{
		log_error("cant get can use channel size:%d", gServer.m_ChannelListener.m_CachedClient.Count());
		ResponseError(ERROR_CHANNEL_FULL);
		return;
	}
	m_OwnerRoom->UnLock();

}
void Client::RequestChangeEquip()
{
	if (m_OwnerRoom == NULL)return;
	Proto::Types::TeamType team = m_Equip.teamtype();
	Proto::Types::CarType car = m_Equip.cartype();
	if (ReadProtoBuffer((google::protobuf::Message*)&m_Equip,read_end - read_position))
	{
		if (team != m_Equip.teamtype())
		{
			Proto::Types::TeamType new_team = m_Equip.teamtype();
			m_Equip.set_teamtype(team);
			if (m_OwnerRoom->ChangeTeam(this, new_team))
			{
				gServer.BroadCastRoomChange(m_OwnerRoom, OFFSET(ROOM_CHANGE_DETAIL));
			}
		}
		else if(car != m_Equip.cartype())
		{
			gServer.BroadCastRoomChange(m_OwnerRoom, OFFSET(ROOM_CHANGE_DETAIL));
		}
		
	}
}
void Client::ResponseError(ushort error)
{
	BeginWrite();
	WriteByte(SM_ERROR);
	WriteUShort(error);
	EndWrite();
}


void Client::OnMessage()
{
	byte cmd = 0;
	ReadByte(cmd);
	switch(cmd)
	{
	case CM_USER_LOGIN:
	{
		ReadString(m_Name, 65);
		gServer.UserLogin(this);
		break;
	}
	case CM_PLAYER_LOGIN:
	{
		ReadULong(m_PlayerId);
		
		gServer.PlayerLogin(this);
		break;
	}
	case CM_RPC_REQUEST:
	{
		RequestRpc();
		break;
	}
	case CM_CREATE_ROOM:
	{
		RequestCreateRoom();
		break;
	}
	case CM_ENTER_ROOM:
	{
		RequestEnterRoom();
		break;
	}
	case CM_LEAVE_ROOM:
	{
		RequestLeaveRoom();
		break;
	}
	case CM_REQUEST_ROOM_INFO:
	{
		RequestRoomInfo();
		break;
	}
	case CM_REQUEST_ROOM_LIST:
	{
		RequestRoomList();
		break;
	}
	case CM_REQUEST_START_GAME:
	{
		RequestStartGame();
		break;
	}
	case CM_REQEUST_CHANGE_EQUIP:
	{
		RequestChangeEquip();
		break;
	}
	case CM_REQUEST_OB_ROOM_LIST:
	{
		RequestOBRoomList();
		break;
	}
	default:
		log_error("cant parse msg : %d uid:%d", cmd,uid);
		this->connection->Disconnect();
		break;
	}
}

bool Client::ThreadSafe()
{
	return true;
}

void Client::OnWrite()
{

}

void Client::OnRevcMessage()
{
	NetworkStream::OnRevcMessage();
}

void Client::OnDisconnect()
{
	log_error("proxy client disconnected uid %d", uid);
	m_IsOnline = false;
	if (m_OwnerRoom)
	{
		Room * r = m_OwnerRoom;
		r->Lock();
		m_OwnerRoom->RemovePlayer(this);
		if(r->m_CurrentPlayerCount>0)gServer.BroadCastRoomChange(r, OFFSET(ROOM_CHANGE_SAMPLE));
		r->UnLock();
	}
	gServer.FreeClient(uid);
	
}

void Client::OnConnected()
{
	log_info("proxy client connected uid %d", uid);
}

void Client::OnReconnected(SocketPoolClinet *c)
{
	this->connection = c;
	this->uid = c->uid;
}

bool Client::IsOnline()
{
	return m_IsOnline;
}


