#pragma once
#include "LuaEngine.h"
#include "TcpListener.h"
#include "TcpConnection.h"
#include "NetworkConnection.h"
#include "objectpool.h"
struct CallBackFunction
{
	int				lua_ref_;
	lua_State*		lua_state_;
	CallBackFunction();
	~CallBackFunction();
	void Init(lua_State* L, int index = -1);
	void Free();
	bool Check() const;
	CallBackFunction& operator=(const CallBackFunction& rhs);
	void operator()()
	{
		int top = lua_gettop(lua_state_);
		lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
		lua_pcall(lua_state_, 0, 0, 0);
		lua_settop(lua_state_,top);
	}

	template<typename P1>
	void operator()(P1 p1)
	{
		int top = lua_gettop(lua_state_);
		lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
		luabridge::push<P1>(lua_state_, p1);
		lua_pcall(lua_state_, 1, 0, 0);
		lua_settop(lua_state_, top);
	}
	template<typename P1, typename P2>
	void operator()(P1 p1,P2 p2)
	{
		int top = lua_gettop(lua_state_);
		lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
		luabridge::push<P1>(lua_state_, p1);
		luabridge::push<P2>(lua_state_, p2);
		lua_pcall(lua_state_, 2, 0, 0);
		lua_settop(lua_state_, top);
	}
	
};
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
	CallBackFunction m_OnConnectedFunc;
	CallBackFunction m_OnDisconnectedFunc;
	CallBackFunction m_OnOnMessageFunc;
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
	CallBackFunction m_AcceptFunc;
};







void RegLuaWebSocket(lua_State* L);