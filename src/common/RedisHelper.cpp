#include "RedisHelper.h"
#include <log.h>

#include <hiredis.h>
#include <async.h>
#include <adapters/libevent.h>
#include <tools.h>
#include <Timer.h>
RedisHelper::RedisHelper() :m_Context(0), m_AsyncContext(NULL)
{

}

RedisHelper::~RedisHelper()
{
	Close();
}

bool RedisHelper::Connnect(const char * addr)
{
	sockaddr_in addr_in;
	ParseSockAddr(addr_in, addr);
	int  m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (0 == connect(m_Socket, (sockaddr*)&addr_in, sizeof(addr_in)))
	{
		m_Context = redisConnectFd(m_Socket);
		if (m_Context == NULL)
		{
			log_error("cant connect redis server : %s", addr);
		}
		return m_Context != NULL;
	}
	return false;
}

bool RedisHelper::AsyncConnect(const char * addr, RedisAsyncCallback * callback)
{
	const char * port_start = strchr(addr, ':');
	char address[256];
	if (port_start)
	{
		memcpy(address, addr, port_start - addr);
		address[port_start - addr] = 0;
		int port = atoi(port_start + 1);
		m_AsyncContext = redisAsyncConnect(address, port);
		if (m_AsyncContext)
		{
			int ret = redisLibeventAttach(m_AsyncContext, Timer::GetEventBase());
			m_AsyncContext->data = this;
			SetAsyncCallback(callback);
		}

		return m_AsyncContext != NULL;
	}
	return false;
}

void RedisHelper::SetAsyncCallback(RedisAsyncCallback * callback)
{
	callback->m_Helper = this;
	m_AsyncContext->data = callback;
	m_AsyncContext->onConnect = RedisAsyncCallback::OnRedisConnect;
	m_AsyncContext->onDisconnect = RedisAsyncCallback::OnRedisDisconnect;
}

void RedisHelper::Close()
{
	if (m_Context)
	{
		redisFree(m_Context);
		m_Context = NULL;
	}
	if (m_AsyncContext)
	{
		redisAsyncFree(m_AsyncContext);
		m_AsyncContext = NULL;
	}
}

void RedisHelper::AsyncScript(RedisAsyncCallback * callback, const char * script)
{
	redisAsyncCommand(m_AsyncContext, RedisAsyncCallback::OnRedisMessage, callback, script);
}

void RedisHelper::AsyncHaveKey(RedisAsyncCallback * callback, int cmd, const char * mapName, const char * key)
{
	if (cmd == REDIS_CMD_HASH)
	{
		AsyncCommand(callback, "HEXISTS %s %s", mapName, key);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		AsyncCommand(callback, "EXISTS %s:%s", mapName, key);
	}
}

void RedisHelper::AsyncDelKey(RedisAsyncCallback * callback, int cmd, const char * mapName, const char * key)
{
	if (cmd == REDIS_CMD_HASH)
	{
		AsyncCommand(callback, "HDEL %s %s", mapName, key);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		AsyncCommand(callback, "	DEL %s:%s", mapName, key);
	}
}

void RedisHelper::AsyncSaveStringValue(RedisAsyncCallback * callback, int cmd, const char * mapName, const char * key, const char * value)
{
	AsyncSaveValue(callback, cmd, mapName, key, value, strlen(value));
}

void RedisHelper::AsyncSaveValue(RedisAsyncCallback * callback, int cmd, const char * mapName, const char * key, const char * value, redis_int len)
{
	if (cmd == REDIS_CMD_HASH)
	{
		AsyncCommand(callback, "HMSET %s %s %b", mapName, key, value, len);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		AsyncCommand(callback, "SET %s:%s %b", mapName, key, value, len);
	}
}

void RedisHelper::AsyncGetValue(RedisAsyncCallback * callback, int cmd, const char * mapName, const char * key)
{
	if (cmd == REDIS_CMD_HASH)
	{
		AsyncCommand(callback, "HGET %s %s", mapName, key);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		AsyncCommand(callback, "GET %s:%s", mapName, key);
	}

}


void RedisHelper::AsyncSaveSortValue(RedisAsyncCallback * callback, const char* mapName, const char* key, redis_int score)
{
	AsyncCommand(callback, "ZADD %s %d %s", mapName, score, key);
}

void RedisHelper::AsyncGetSortValue(RedisAsyncCallback * callback, const char * mapName, redis_int start, redis_int end)
{
	AsyncCommand(callback, "ZRANGE %s %d %d WITHSCORES", mapName, start, end);
}

void RedisHelper::AsyncCountSortValue(RedisAsyncCallback * callback, const char * mapName, redis_int start, redis_int end)
{
	if (end - start >= 0)
	{
		AsyncCommand(callback, "ZCOUNT %s %d %d", mapName, start, end);
	}
	else {
		AsyncCommand(callback, "ZCARD %s", mapName);
	}
}

void RedisHelper::AsyncExpireKey(RedisAsyncCallback * callback, int cmd, const char * mapName, const char * key, redis_int s)
{
	if (cmd == REDIS_CMD_HASH)
	{
		AsyncCommand(callback, "EXPIRE %s %llu", mapName, s);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		AsyncCommand(callback, "EXPIRE %s:%s %llu", mapName, key, s);
	}
}


bool RedisHelper::ScriptCmd(RedisResponse * res, const char * cmd, const char * script)
{
	res->Free();
	char format[256] = { 0 };
	if (strlen(script) == 0)
	{
		sprintf(format, "%s", cmd);
		res->m_Reply = (redisReply*)redisCommand(m_Context, format);
	}
	else
	{
		sprintf(format, "%s %s", cmd,"%s");
		res->m_Reply = (redisReply*)redisCommand(m_Context, format, script);
	}
	
	res->isChild = false;
	return res->Valid();
}

bool RedisHelper::ScriptEval(RedisResponse *res, const char* cmd, const char* script, int argc, const char* arg)
{
	res->Free();
	char format[512] = { 0 };
	if (argc == 0)
	{
		sprintf(format, "%s %s 0", cmd, "%s");
		
	}
	else
	{
		sprintf(format, "%s %s %d %s", cmd, "%s",argc,arg);
	}
	res->m_Reply = (redisReply*)redisCommand(m_Context, format, script);
	res->isChild = false;
	return res->Valid();
}

bool RedisHelper::HaveKey(int cmd, const char * mapName, const char * key)
{
	RedisResponse res;
	if (cmd == REDIS_CMD_HASH)
	{
		Command(&res, "HEXISTS %s %s", mapName, key);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		Command(&res, "	EXISTS %s:%s", mapName, key);
	}
	return res.Valid() && res.Integer() > 0;

}

bool RedisHelper::DelKey(int cmd, const char * mapName, const char * key)
{
	RedisResponse res;
	if (cmd == REDIS_CMD_HASH)
	{
		Command(&res, "HDEL %s %s", mapName, key);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		Command(&res, "	DEL %s:%s", mapName, key);
	}
	return res.Valid() && res.Integer() > 0;
}

bool RedisHelper::SaveValue(int cmd, RedisResponse * res, const char * mapName, const char * key, const char * value, redis_int len)
{
	if (cmd == REDIS_CMD_HASH)
	{
		Command(res, "HMSET %s %s %b", mapName, key, value, len);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		Command(res, "SET %s:%s %b", mapName, key, value, len);
	}

	return res->Valid();
}

bool RedisHelper::SaveStringValue(int cmd, RedisResponse * res, const char * mapName, const char * key, const char * value)
{
	return SaveValue(cmd,res,mapName,key,value,strlen(value));
}

bool RedisHelper::GetValue(int cmd, RedisResponse * res, const char * mapName, const char * key)
{
	if (cmd == REDIS_CMD_HASH)
	{
		Command(res, "HGET %s %s", mapName, key);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		Command(res, "GET %s:%s", mapName, key);
	}

	return res->Valid();
}


bool RedisHelper::SaveSortValue(RedisResponse *res, const char* mapName, const char* key, redis_int score)
{
	Command(res, "ZADD %s %d %s", mapName, score, key);
	return res->Valid();
}

bool RedisHelper::GetSortValue(RedisResponse * res, const char * mapName, redis_int start, redis_int end)
{
	Command(res, "ZRANGE %s %d %d WITHSCORES", mapName, start, end);
	return res->Valid();
}

int RedisHelper::CountSortValue(const char * mapName, redis_int start, redis_int end)
{
	RedisResponse res;
	if (end - start >= 0)
	{
		Command(&res, "ZCOUNT %s %d %d", mapName, start, end);
	}
	else {
		Command(&res, "ZCARD %s", mapName);
	}
	if (res.Valid())
	{
		return res.Integer();
	}
	return 0;
}

bool RedisHelper::ExpireKey(int cmd, const char * mapName, const char * key, int s)
{
	RedisResponse res;
	if (cmd == REDIS_CMD_HASH)
	{
		Command(&res, "expire %s %d", mapName, s);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		Command(&res, "expire %s:%s %d", mapName, key, s);
	}
	return res.Valid() && res.Integer() > 0;
}


void RedisHelper::Command(RedisResponse *res, const char *format, ...)
{
	res->Free();
	va_list ap;
	va_start(ap, format);
	void *reply = NULL;
	reply = redisvCommand(m_Context, format, ap);
	res->m_Reply = (redisReply*)reply;
	va_end(ap);
	res->isChild = false;



}

void RedisHelper::AsyncCommand(RedisAsyncCallback * callback, const char * format, ...)
{
	callback->m_Helper = this;
	va_list ap;
	va_start(ap, format);
	int ret = redisvAsyncCommand(m_AsyncContext, RedisAsyncCallback::OnRedisMessage, callback, format, ap);
	va_end(ap);
}

RedisResponse::RedisResponse() :m_Reply(NULL), isChild(false)
{
}

RedisResponse::~RedisResponse()
{
	Free();
}

bool RedisResponse::Valid()
{
	return m_Reply != NULL && m_Reply->type != REDIS_REPLY_NIL;
}

int RedisResponse::Type()
{
	if (Valid())return m_Reply->type;
	return -1;
}

int RedisResponse::Length()
{
	if (Valid())return m_Reply->len;
	return 0;
}

int RedisResponse::Integer()
{
	if (Valid())return m_Reply->integer;
	return 0;
}

const char * RedisResponse::String()
{
	if (Valid())return m_Reply->str;
	return NULL;
}

redis_int RedisResponse::ElementsCount()
{
	if (Valid())return m_Reply->elements;
	return 0;
}

bool RedisResponse::Element(RedisResponse & res, int index)
{
	if (Valid() && index >= 0 && index < m_Reply->elements)
	{
		res.m_Reply = m_Reply->element[index];
		res.isChild = true;
		return true;
	}
	return false;
}

void RedisResponse::Free()
{
	if (!isChild && m_Reply)
	{
		freeReplyObject(m_Reply);
		m_Reply = NULL;
	}
}

void RedisAsyncCallback::OnRedisMessage(struct redisAsyncContext*, void* reply, void* arg)
{
	RedisAsyncCallback* callback = (RedisAsyncCallback*)arg;
	if (reply && callback)
	{
		RedisResponse res;
		res.m_Reply = (redisReply*)reply;
		callback->OnMessage(&res);
		res.m_Reply = NULL;
	}
}

void RedisAsyncCallback::OnRedisConnect(const redisAsyncContext *context, int status)
{
	RedisAsyncCallback* callback = (RedisAsyncCallback*)context->data;
	if (callback)callback->OnConnect(status);
}

void RedisAsyncCallback::OnRedisDisconnect(const redisAsyncContext *context, int status)
{
	RedisAsyncCallback* callback = (RedisAsyncCallback*)context->data;
	if (callback)callback->OnDisconnect(status);
}

