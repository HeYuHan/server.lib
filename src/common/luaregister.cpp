#include "luaregister.h"
#include "LuaWebSocket.h"
#include "Timer.h"
#include "LuaBridge/LuaFunction.h"
#include "LuaBridge/LuaTable.h"
#include "RedisHelper.h"
#include "log.h"
#include <httplib.h>
#include "MessageQueue.h"
#include <thread>
#include "HttpClient.h"
#include "FileReader.h"
using namespace luabridge;
using namespace Core;


class LuaHttp:public LuaRegister,public IThreadMessageHadle,public HttpClient
{
public:
	LuaHttp()
	{

	}
	void AsyncGet(const char* path)
	{
		std::thread t([&]
		{
			this->Get(path);
			this->SendMessageToMainThread(0);
		});
		t.detach();
	}
	void AsyncPost(const char* path, const char* data, const char* type)
	{
		std::thread t([&]
		{
			this->Post(path, data, type);
			this->SendMessageToMainThread(0);
		});
		t.detach();
	}
	bool Get(const char* path)
	{
		return HttpClient::Get(path);
	}
	bool Post(const char* path, const char* data, const char* type)
	{
		return HttpClient::Post(path, data, type);
	}
	const char* GetBody()
	{
		return m_Content.c_str();
	}
	int GetStatus()
	{
		return m_ResultCode;
	}

private:
	LuaFunction m_LuaCallback;
	LuaObject m_LuaSender;

	// 通过 IThreadMessageHadle 继承
	virtual void OnThreadMessage(unsigned int id) override
	{
		if (m_LuaCallback.IsValid())
		{
			m_LuaCallback(this, &m_LuaSender);
		}
	}
	// 通过 LuaRegister 继承
	virtual int OnRegister(lua_State * L) override
	{
		if (lua_istable(L, -1))
		{
			lua_getfield(L, -1, "OnMessage");
			if (lua_isfunction(L, -1))
			{
				m_LuaCallback.Init(L);
			}
			else
			{
				log_error("%s", "cant find OnMessage function");
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
	// 通过 LuaRegister 继承
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
class LuaRedisAsyncCallback :public RedisAsyncCallback
{
private:
	LuaFunction m_LuaOnMessage;
	LuaFunction m_LuaOnConnect;
	LuaFunction m_LuaOnDisconnect;
	LuaObject m_LuaSender;
public:
	// 通过 RedisAsyncCallback 继承
	virtual void OnMessage(RedisResponse * res) override
	{
		if (m_LuaOnMessage.IsValid())
		{
			m_LuaOnMessage(res, &m_LuaSender);
		}
	}
	virtual void OnConnect(int status) override
	{
		if (m_LuaOnConnect.IsValid())
		{
			m_LuaOnConnect(status, &m_LuaSender);
		}
	}
	virtual void OnDisconnect(int status) override
	{
		if (m_LuaOnDisconnect.IsValid())
		{
			m_LuaOnDisconnect(status, &m_LuaSender);
		}
	}
	int OnRegister(lua_State * L)
	{
		if (lua_istable(L, -1))
		{
			lua_getfield(L, -1, "OnMessage");
			if (lua_isfunction(L, -1))
			{
				m_LuaOnMessage.Init(L);
			}
			else
			{
				log_error("%s", "cant find OnMessage function");
			}
			lua_getfield(L, -1, "OnConnect");
			if (lua_isfunction(L, -1))
			{
				m_LuaOnConnect.Init(L);
			}
			else
			{
				log_error("%s", "cant find OnConnect function");
			}
			lua_getfield(L, -1, "OnDisconnect");
			if (lua_isfunction(L, -1))
			{
				m_LuaOnDisconnect.Init(L);
			}
			else
			{
				log_error("%s", "cant find OnDisconnect function");
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

void RegRedis(lua_State *L)
{
	getGlobalNamespace(L)
		.beginClass<RedisAsyncCallback>("NativeRedisCallback")
		.endClass()
		.deriveClass<LuaRedisAsyncCallback, RedisAsyncCallback>("RedisCallback")
		.addConstructor<void(*) (void)>()
		.addCFunction("OnRegister", &LuaRedisAsyncCallback::OnRegister)
		.endClass()
		.beginClass<RedisResponse>("RedisResponse")
		.addConstructor<void(*) (void)>()
		.addFunction("Valid", &RedisResponse::Valid)
		.addFunction("Type", &RedisResponse::Type)
		.addFunction("Length", &RedisResponse::Length)
		.addFunction("Integer", &RedisResponse::Integer)
		.addFunction("String", &RedisResponse::String)
		.addFunction("ElementsCount", &RedisResponse::ElementsCount)
		.addFunction("Element", &RedisResponse::Element)
		.endClass()
		.beginClass<RedisHelper>("RedisHelper")
		.addConstructor<void(*) (void)>()
		.addFunction("Connect", &RedisHelper::Connnect)
		.addFunction("Close", &RedisHelper::Close)
		.addFunction("HaveKey", &RedisHelper::HaveKey)
		.addFunction("DelKey", &RedisHelper::DelKey)
		.addFunction("SaveValue", &RedisHelper::SaveStringValue)
		.addFunction("GetValue", &RedisHelper::GetValue)
		.addFunction("SaveSortValue", &RedisHelper::SaveSortValue)
		.addFunction("GetSortValue", &RedisHelper::GetSortValue)
		.addFunction("CountSortValue", &RedisHelper::CountSortValue)
		.addFunction("ExpireKey", &RedisHelper::ExpireKey)
		.addFunction("Keys", &RedisHelper::Keys)
		.addFunction("ScriptEval",&RedisHelper::ScriptEval)
		.addFunction("ScriptCmd", &RedisHelper::ScriptCmd)

		.addFunction("AsyncConnect", &RedisHelper::AsyncConnect)
		.addFunction("AsyncHaveKey", &RedisHelper::AsyncHaveKey)
		.addFunction("AsyncDelKey", &RedisHelper::AsyncDelKey)
		.addFunction("AsyncSaveValue", &RedisHelper::AsyncSaveStringValue)
		.addFunction("AsyncGetValue", &RedisHelper::AsyncGetValue)
		.addFunction("AsyncSaveSortValue", &RedisHelper::AsyncSaveSortValue)
		.addFunction("AsyncGetSortValue", &RedisHelper::AsyncGetSortValue)
		.addFunction("AsyncCountSortValue", &RedisHelper::AsyncCountSortValue)
		.addFunction("AsyncExpireKey", &RedisHelper::AsyncExpireKey)
		.addFunction("AsyncScript", &RedisHelper::AsyncScript)
		.endClass();
}
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
void RegHttp(lua_State *L)
{
	getGlobalNamespace(L)
		.deriveClass<LuaHttp, LuaRegister>("Http")
		.addConstructor<void(*) (void)>()
		.addFunction("Get", &LuaHttp::Get)
		.addFunction("Post", &LuaHttp::Post)
		.addFunction("AsyncGet", &LuaHttp::AsyncGet)
		.addFunction("AsyncPost", &LuaHttp::AsyncPost)
		.addFunction("Body", &LuaHttp::GetBody)
		.addFunction("Status",&LuaHttp::GetStatus)
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
int RegFieWriter(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<AsyncFileWriter>("AsyncFileWriter")
		.addConstructor<void(*) (void)>()
		.addFunction("Create", &AsyncFileWriter::Create)
		.addFunction("Write", &AsyncFileWriter::Write)
		.addFunction("PushContent", &AsyncFileWriter::PushContent)
		.addFunction("PushContentLine", &AsyncFileWriter::PushContentLine)
		.addFunction("Close", &AsyncFileWriter::Close)
		.endClass();
	return 0;
	return 0;
}
int len(const char* str)
{
	return strlen(str);
}
void lua_log_error(const char* msg)
{
	log_error("%s", msg);
}
void lua_log_debug(const char* msg)
{
	log_debug("%s", msg);
}
void lua_log_info(const char* msg)
{
	log_info("%s", msg);
}
int bit_left(int value, int size)
{
	return value << size;
}
int bit_right(int value, int size)
{
	return value >> size;
}
int bit_and(int value1, int value2)
{
	return value1 & value2;
}
int bit_or(int value1, int value2)
{
	return value1 | value2;
}
extern "C"
{
	int luaopen_cjson(lua_State *l);
	int luaopen_cjson_safe(lua_State *l);
}

int RegAllNative(lua_State * L)
{
	/////////////////////////////////////////////////////////////
	//begin
	getGlobalNamespace(L)
		.addFunction("bit_left",bit_left)
		.addFunction("bit_right", bit_right)
		.addFunction("bit_and", bit_and)
		.addFunction("bit_or",bit_or)
		.addFunction("strlen", len)
		.addFunction("log_error", lua_log_error)
		.addFunction("log_debug", lua_log_debug)
		.addFunction("log_info", lua_log_info)
		.addFunction("InitMessageQueue",&MainThreadMessageQueue::Init)
		.beginClass<LuaRegister>("LuaRegister")
		.addCFunction("OnRegister",&LuaRegister::OnRegister)
		.endClass();

	//timer
	RegTimer(L);
	//websocket
	RegLuaWebSocket(L);
	RegRedis(L);
	RegHttp(L);
	RegFieWriter(L);
	luaopen_cjson(L);
	return 0;
}
