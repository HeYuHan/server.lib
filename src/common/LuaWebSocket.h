#pragma once
#include "LuaEngine.h"
#include "TcpListener.h"
#include "TcpConnection.h"
#include "NetworkConnection.h"
#include "objectpool.h"
class LuaWebSocketListenner;
class LuaWebSocketClient:public Core::TcpConnection,public Core::NetworkStream
{
public:
	LuaWebSocketClient();
public:
	// 通过 TcpConnection 继承
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void Disconnect();
	// 通过 NetworkStream 继承
	virtual void OnMessage() override;
	virtual bool ThreadSafe() override;
	void Send(const char* msg);
public:
	int RegisterCallBack(lua_State* L);
public:
	Core::uint uid;
	Core::LuaFunction m_OnConnectedFunc;
	Core::LuaFunction m_OnDisconnectedFunc;
	Core::LuaFunction m_OnOnMessageFunc;
	Core::LuaObject m_LuaObj;
	LuaWebSocketListenner* m_Listenner;


};
class LuaWebSocketListenner :public TcpListener
{
public:
	LuaWebSocketListenner();
	~LuaWebSocketListenner();
public:
	// 通过 TcpListener 继承
	virtual void OnTcpAccept(evutil_socket_t socket, sockaddr *) override;
	bool Listen(const char* addr, int size);
	LuaWebSocketClient* GetClient(Core::uint uid);
	void FreeClient(Core::uint uid);
	int RegisterCallBack(lua_State* L);
	void CallbackAccept(LuaWebSocketClient* c);
private:

private:
	Core::ObjectPool<LuaWebSocketClient> m_CachedClient;
	Core::LuaFunction m_AcceptFunc;
};