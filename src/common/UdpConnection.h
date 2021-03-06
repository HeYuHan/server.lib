#pragma once
#ifdef RAKNET



#ifndef __UDPCONNECTION_H__
#define __UDPCONNECTION_H__
#include "NetworkConnection.h"
#include <MessageIdentifiers.h>
#include <RakPeerInterface.h>
#include <RakNetTypes.h>
#include <BitStream.h>
#include <PacketLogger.h>
#include <RakNetTypes.h>
BEGIN_NS_CORE
using namespace RakNet;
class UdpConnection :public NetworkConnection
{
public:
	UdpConnection();
	~UdpConnection();
	virtual void Update(float time);
	virtual int Read(void* data, int size);
	virtual int Send(void* data, int size);
	bool Connect(const char* ip, int port);
	void InitServerSocket(RakPeerInterface* server, SystemAddress address);
	void OnServerMessage(Packet* p);
	void Disconnect();
	void KeepAlive();
public:
	
	bool m_IsServer;
	RakPeerInterface *m_Socket;
	SystemAddress m_SystemAddress;
	Packet* m_MessagePacket;
	

};

END_NS_CORE

#endif // !__UDPCONNECTION_H__
#endif // RAKNET
