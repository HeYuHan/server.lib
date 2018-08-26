#include "luaregister.h"
#include "LuaWebSocket.h"
#include "Timer.h"
#include "LuaBridge/LuaFunction.h"
#include "LuaBridge/LuaTable.h"
#include "log.h"
using namespace luabridge;
using namespace Core;


class LuaTimer :public Timer,public LuaRegister
{
public:
	LuaFunction m_LuaCallback;
	LuaObject m_LuaSender;
	static void TimerUpdate(float time, void *arg)
	{
		LuaTimer *timer = (LuaTimer*)arg;
		if (timer)timer->Update(time);
	}
	void Update(float t)
	{
		if (m_LuaCallback.IsValid())
		{
			m_LuaCallback(t,&m_LuaSender);
		}
	}
	void Stop2() { Stop(); }
	void Begin2()
	{
		Begin();
	}
	bool Init2(float time,bool loop)
	{
		if (m_LuaCallback.IsValid())
		{
			Init(time, LuaTimer::TimerUpdate, this, loop);
			return true;
		}
		return false;
	}
	// Í¨¹ý LuaRegister ¼Ì³Ð
	virtual int OnRegister(lua_State * L) override
	{
		if (lua_istable(L, -1))
		{
			lua_getfield(L, -1, "OnUpdate");
			if (lua_isfunction(L, -1))
			{
				m_LuaCallback.Init(L);
			}
			else
			{
				log_error("%s", "cant find OnUpdate function");
			}
			m_LuaSender.Init(L);
		}
		else
		{
			log_error("%s", "can't not find lua object\n");
		}
		return 0;
	}
	
};
void RegTimer(lua_State * L)
{
	getGlobalNamespace(L)
		.deriveClass<LuaTimer, LuaRegister>("Timer")
		.addStaticFunction("Loop", &Timer::Loop)
		.addConstructor<void(*) (void)>()
		.addFunction("Begin",&LuaTimer::Begin2)
		.addFunction("Stop", &LuaTimer::Stop2)
		.addFunction("Init", &LuaTimer::Init2)
		.endClass();
}

int RegLuaWebSocket(lua_State * L)
{
	luabridge::getGlobalNamespace(L)
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
		.endClass();
	return 0;
}
int RegAllNative(lua_State * L)
{
	/////////////////////////////////////////////////////////////
	//begin
	getGlobalNamespace(L)
		.beginClass<LuaRegister>("LuaRegister")
		.addCFunction("OnRegister",&LuaRegister::OnRegister)
		.endClass();

	//timer
	RegTimer(L);
	//websocket
	RegLuaWebSocket(L);

	return 0;
}
