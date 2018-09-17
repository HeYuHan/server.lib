#pragma once
#ifndef __NETWORKCONNECTION_H__
#define __NETWORKCONNECTION_H__
#include "common.h"
#include "Vector3.h"
#include "ThreadPool2.h"
namespace google
{
	namespace protobuf
	{
		class Message;
	}
}
BEGIN_NS_CORE
class NetworkConnection;
typedef enum
{
	TCP_SOCKET=1,UDP_SOCKET,WEB_SOCKET
}SocketType;
typedef enum
{
	READERROR = 0, WRITEERROR = 1
}NETERR;
class NetworkStream
{
public:
	NetworkStream(int send_buff_size = 1024 * 512, int read_buff_size = 1024 * 512);
	~NetworkStream();
	virtual void OnMessage()=0;
	virtual bool ThreadSafe() = 0;
	void Reset();
	void OnRevcMessage(bool parse=true);
	void ParseMessage();
public:
	void WriteBool(bool b);
	void WriteByte(byte data);
	void WriteChar(char data);
	void WriteShort(short data);
	void WriteUShort(ushort data);
	void WriteInt(int data);
	void WriteUInt(uint data);
	void WriteFloat(float data);
	void WriteLong(int64 data);
	void WriteULong(uint64 data);
	void WriteString(const char* str);
	void WriteData(const void* data, int count);
	void WriteVector3(Vector3 &v3);
	void WriteShortQuaternion(Quaternion &rot);
#ifdef PROTO_BUFF

	void WriteProtoBufferAutoSize(google::protobuf::Message *message);
	void WriteProtoBuffer(google::protobuf::Message *message);
#endif // PROTO_BUFF

	
	void BeginWrite();
	void EndWrite();
	//////////////////////////////////////////////////////////////
	//read data
public:
	void ReadByte(byte &data);
	void ReadByte(char &data);
	void ReadShort(short &data);
	void ReadUShort(ushort &data);
	void ReadInt(int &data);
	void ReadUInt(uint &data);
	void ReadFloat(float &data);
	void ReadLong(int64 &data);
	void ReadULong(uint64 &data);
	int ReadString(char* str, int size);
	void ReadData(void* data, int count);
	void ReadVector3(Vector3 &v3);
	void ReadShortQuaternion(Quaternion &rot);
#ifdef PROTO_BUFF
	bool ReadProtoBuffer(google::protobuf::Message *message,int size);
	bool ReadProtoBufferAutoSize(google::protobuf::Message *message);
#endif
public:
	NetworkConnection* connection;
public:
	char* write_buff;
	char* write_position;
	char* write_end;
	char* write_buff_end;

	char* read_offset;
	char* read_buff;
	char* read_end;
	char* read_position;
	char* read_buff_end;
private:
	char* web_frame;
	ThreadObject m_ReadLock;
	ThreadObject m_WriteLock;
};


class NetworkConnection
{
public:
	NetworkConnection();
	~NetworkConnection();
	//virtual int SetEvent(struct event_base *) { return -1; }
	virtual void Update(float time) = 0;
	virtual int Read(void* data, int size) = 0;
	virtual int Send(void* data, int size) = 0;
	virtual void OnConnected() = 0;
	virtual void OnDisconnected() = 0;
	virtual void Disconnect()=0;
	virtual int GetSocket() { return -1; }
	
public:
	NetworkStream* stream;
	SocketType m_Type;

};
END_NS_CORE
#endif
