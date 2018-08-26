#include "LuaWebSocket.h"
#include <LuaBridge\LuaBridge.h>
#include <Timer.h>
using namespace luabridge;

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
	if(m_AcceptFunc.IsValid())m_AcceptFunc(c);
}

LuaWebSocketClient::LuaWebSocketClient():m_Listenner(NULL)
{
}

void LuaWebSocketClient::OnConnected()
{
	if(m_OnConnectedFunc.IsValid())m_OnConnectedFunc(&m_LuaObj);
}

void LuaWebSocketClient::OnDisconnected()
{
	if(m_OnDisconnectedFunc.IsValid())m_OnDisconnectedFunc(&m_LuaObj);
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
	if (m_OnOnMessageFunc.IsValid())
	{
		char c = read_end[0];
		read_end[0] = 0;
		const char* msg = read_position;
		m_OnOnMessageFunc(msg, &m_LuaObj);
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
	if (lua_istable(L,-1))
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
		lua_getfield(L, -1, "OnDisconnected");
		if (lua_isfunction(L, -1))
		{
			m_OnDisconnectedFunc.Init(L);
		}
		else
		{
			log_error("%s", "cant find OnDisconnected function");
		}
		lua_getfield(L, -1, "OnMessage");
		if (lua_isfunction(L, -1))
		{
			m_OnOnMessageFunc.Init(L);
		}
		else
		{
			log_error("%s", "cant find OnMessage function");
		}
		m_LuaObj.Init(L);
	}
	else
	{
		log_error("%s", "can't not find lua object\n");
	}
	
	return 0;
}