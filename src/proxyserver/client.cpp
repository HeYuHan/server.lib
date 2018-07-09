#include "client.h"
#include "server.h"
#include "message_type.h"
#include <google/protobuf/message.h>
#include "ClientData.pb.h"
using namespace Proto;

void ClientRpcCallBack(RPCRequestPack<google::protobuf::Message, google::protobuf::Message>* origin)
{
	Client *c = static_cast<Client*>(origin->m_UserData[0]);
	c->BeginWrite();
}

Client::Client():m_IsOnline(false),uid(0),m_AccountId(0),m_PlayerId(0)
{

}
void Client::Init()
{
	m_IsOnline = false;
	m_AccountId = 0;
	m_PlayerId = 0;
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
void Client::OnMessage()
{
	byte cmd = 0;
	ReadByte(cmd);
	switch(cmd)
	{
	case CM_LOGIN:
	{
		ReadString(m_Name, 65);
		gServer.Login(this);
		break;
	}
	case CM_RPC_REQUEST:
	{
		RequestRpc();
		break;
	}
	default:
		log_error("cant parse msg : %d", cmd);
		this->connection->Disconnect();
		break;
	}
}

bool Client::ThreadSafe()
{
	return true;
}

uint Client::GetUid()
{
	return uint();
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
	gServer.FreeClient(uid);
}

void Client::OnConnected()
{

}

void Client::OnReconnected(SocketPoolClinet *c)
{
	this->connection = c;
	this->uid = c->uid;
}

void Client::WriteProtoBuffer(google::protobuf::Message *message)
{
	int size = message->ByteSize();
	int empty_size = write_buff_end - write_end;
	if (empty_size < size)
	{
		log_error("buffer size too small msg size:%d", size);
		throw NETERR::WRITEERROR;
	}
	else
	{
		message->SerializeToArray(write_end, empty_size);
		write_end += size;
	}
}
