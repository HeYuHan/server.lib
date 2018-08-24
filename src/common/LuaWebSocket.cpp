#include "LuaWebSocket.h"
#include <LuaBridge\LuaBridge.h>
#include <Timer.h>
using namespace luabridge;
void RegLuaWebSocket(lua_State * L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Net")
		.beginClass<LuaWebSocketListenner>("WebListenner")
		.addConstructor<void(*) (void)>()
		.addFunction("Listen", &LuaWebSocketListenner::Listen)
		.addCFunction("RegisterCallBack", &LuaWebSocketListenner::RegisterCallBack)
		.addFunction("GetClient", &LuaWebSocketListenner::GetClient)
		.addFunction("FreeClient", &LuaWebSocketListenner::FreeClient)
		.endClass()
		.beginClass<LuaWebSocketClient>("WebClient")
		.addData("uid", &LuaWebSocketClient::uid)
		.addCFunction("RegisterCallBack", &LuaWebSocketClient::RegisterCallBack)
		.addFunction("Disconnect", &LuaWebSocketClient::Disconnect)
		.addFunction("Send", &LuaWebSocketClient::Send)
		.endClass()
		.endNamespace();
}

LuaWebSocketListenner::LuaWebSocketListenner()
{
}

LuaWebSocketListenner::~LuaWebSocketListenner()
{
	
}

void LuaWebSocketListenner::OnTcpAccept(evutil_socket_t socket, sockaddr * addr)
{
	LuaWebSocketClient *c = m_CachedClient.Allocate();
	if (NULL == c)
	{
		evutil_closesocket(socket);
		log_error("cant allocate more cliet pool size : %d", m_CachedClient.Size());
		return;
	}
	c->m_Listenner = this;
	c->connection = c;
	c->stream = c;
	c->stream->Reset();
	CallbackAccept(c);
	c->m_Type = Core::WEB_SOCKET;
	c->InitSocket(socket, addr, Timer::GetEventBase());
	
}

bool LuaWebSocketListenner::Listen(const char * addr, int size)
{
	if (!m_CachedClient.Initialize(size))
	{
		log_error("init cliet pool failed size %d", size);
		return false;
	}
	return TcpListener::CreateTcpServer(addr,size);
}

LuaWebSocketClient * LuaWebSocketListenner::GetClient(Core::uint uid)
{
	return m_CachedClient.Get(uid);
}

void LuaWebSocketListenner::FreeClient(Core::uint uid)
{
	m_CachedClient.Free(uid);
}

int LuaWebSocketListenner::RegisterCallBack(lua_State * L)
{
	int top = lua_gettop(L);
	if (lua_istable(L, -1))
	{
		lua_getfield(L, -1, "OnAccept");
		if (lua_isfunction(L, -1))
		{
			m_AcceptFunc.Init(L);
		}
		else
		{
			log_error("%s", "cant find accept function");
		}
	}
	else
	{
		log_error("%s", "can't not find lua object\n");
	}
	lua_settop(L, top);
	return 0;
}

void LuaWebSocketListenner::CallbackAccept(LuaWebSocketClient * c)
{
	if(m_AcceptFunc.Check())m_AcceptFunc(c);
}

LuaWebSocketClient::LuaWebSocketClient():m_Listenner(NULL)
{
}

void LuaWebSocketClient::OnConnected()
{
	if(m_OnConnectedFunc.Check())m_OnConnectedFunc(this);
}

void LuaWebSocketClient::OnDisconnected()
{
	if(m_OnDisconnectedFunc.Check())m_OnDisconnectedFunc(this);
	this->stream = NULL;
	this->connection = NULL;
	this->m_Listenner->FreeClient(this->uid);
	this->m_Listenner = NULL;
}

void LuaWebSocketClient::Disconnect()
{
	TcpConnection::Disconnect();
}

void LuaWebSocketClient::OnMessage()
{
	if (m_OnOnMessageFunc.Check())
	{
		char c = read_end[0];
		read_end[0] = 0;
		m_OnOnMessageFunc(this, read_position);
		read_end[0] = c;
	}

}

bool LuaWebSocketClient::ThreadSafe()
{
	return false;
}

void LuaWebSocketClient::Send(const char * msg)
{
	BeginWrite();
	WriteData(msg, strlen(msg));
	EndWrite();
}

int LuaWebSocketClient::RegisterCallBack(lua_State * L)
{
	int top = lua_gettop(L);
	if (lua_istable(L, -1))
	{
		lua_getfield(L, -1, "OnConnected");
		if (lua_isfunction(L, -1))
		{
			m_OnConnectedFunc.Init(L);
		}
		else
		{
			log_error("%s", "cant find OnConnected function");
		}
		lua_pop(L, 1);
		lua_getfield(L, -1, "OnDisconnected");
		if (lua_isfunction(L, -1))
		{
			m_OnDisconnectedFunc.Init(L);
		}
		else
		{
			log_error("%s", "cant find OnDisconnected function");
		}
		lua_pop(L, 1);
		lua_getfield(L, -1, "OnMessage");
		if (lua_isfunction(L, -1))
		{
			m_OnOnMessageFunc.Init(L);
		}
		else
		{
			log_error("%s", "cant find OnMessage function");
		}
		lua_pop(L, 1);
	}
	else
	{
		log_error("%s", "can't not find lua object\n");
	}
	lua_settop(L, top);
	return 0;
}

CallBackFunction::CallBackFunction():lua_ref_(LUA_REFNIL), lua_state_(NULL)
{
}

CallBackFunction::~CallBackFunction()
{
	Free();
}

void CallBackFunction::Init(lua_State * L, int index)
{
	Free();
	int oldTop = lua_gettop(L);

	if (lua_istable(L, index) || lua_isfunction(L, index))
	{
		lua_ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	else
	{
		lua_ref_ = LUA_REFNIL;
		log_error("%s", "can't not find lua object\n");
	}
	lua_state_ = L;
	lua_settop(L, oldTop);
}

void CallBackFunction::Free()
{
	if (Check())
	{
		luaL_unref(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
		lua_ref_ = 0;
		lua_state_ = NULL;
	}
}

CallBackFunction & CallBackFunction::operator=(const CallBackFunction & rhs)
{
	if (rhs.Check())
	{
		lua_rawgeti(rhs.lua_state_, LUA_REGISTRYINDEX, rhs.lua_ref_);
		Init(rhs.lua_state_, -1);
	}
	else
	{
		lua_ref_ = LUA_REFNIL;
	}
	return *this;
}

bool CallBackFunction::Check() const
{
	return (lua_ref_ != LUA_REFNIL) && (lua_state_ != NULL);
}
