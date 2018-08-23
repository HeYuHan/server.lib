#pragma once
#ifndef __LOGIN_SERVER_H__
#define __LOGIN_SERVER_H__
#include <RedisHelper.h>
#include <BaseServer.h>
#include <TcpListener.h>

class Server:public BaseServer,public TcpListener
{
public:
	Server();
	~Server();
	virtual bool Init();
	virtual int Run();
public:
	// Í¨¹ý TcpListener ¼Ì³Ð
	virtual void OnTcpAccept(evutil_socket_t socket, sockaddr *) override;
public:
	RedisHelper m_DBHelper;
};
extern Server gServer;
#endif // !__LOGIN_SERVER_H__

