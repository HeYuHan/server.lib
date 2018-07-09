#pragma once
#ifndef __NETWORKCONNECTION_H__
#define __NETWORKCONNECTION_H__
#include "common.h"
#include "Vector3.h"
#include "ThreadPool2.h"
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
typedef enum 
{
	READ=1<<1,WRITE=1<<2
}StreamLock;
class NetworkStream
{
public:
	NetworkStream(int send_buff_size = 1024 * 512, int read_buff_size = 1024 * 512);
	~NetworkStream();
	virtual void OnMessage()=0;
	virtual bool ThreadSafe() = 0;
	void Reset();
	virtual void OnRevcMessage();
public:
	void WriteBool(bool b);
	void WriteByte(byte data);
	void WriteChar(char data);
	void WriteShort(short data);
	void WriteUShort(ushort data);
	void WriteInt(int data);
	void WriteUInt(uint data);
	void WriteFloat(float data);
	void WriteLong(long data);
	void WriteULong(ulong data);
	void WriteString(const char* str);
	void WriteData(const void* data, int count);
	void WriteVector3(Vector3 &v3);
	void WriteShortQuaternion(Quaternion &rot);
	void BeginWrite();
	void EndWrite();
	//////////////////////////////////////////////////////////////
	//read data
public:
	void Lock(int type);
	void UnLock(int type);
	void ReadByte(byte &data);
	void ReadByte(char &data);
	void ReadShort(short &data);
	void ReadUShort(ushort &data);
	void ReadInt(int &data);
	void ReadUInt(uint &data);
	void ReadFloat(float &data);
	void ReadLong(long &data);
	void ReadULong(ulong &data);
	int ReadString(char* str, int size);
	void ReadData(void* data, int count);
	void ReadVector3(Vector3 &v3);
	void ReadShortQuaternion(Quaternion &rot);
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
	uint uid;
private:
	char* web_frame;
	MutexLock m_WriteLock;
	MutexLock m_ReadLock;
};


class NetworkConnection
{
public:
	NetworkConnection();
	~NetworkConnection();
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
