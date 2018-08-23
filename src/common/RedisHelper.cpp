#include "RedisHelper.h"

#include <log.h>
#include <tools.h>
#include <hiredis.h>
RedisHelper::RedisHelper():m_Context(0)
{

}

RedisHelper::~RedisHelper()
{
}

bool RedisHelper::Connnect(const char * addr)
{
	sockaddr_in addr_in;
	if (!ParseSockAddr(addr_in, addr))
	{
		return false;
	}
	int m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (0 == connect(m_Socket, (sockaddr*)&addr_in, sizeof(addr_in)))
	{
		m_Context = redisConnectFd(m_Socket);
		if (m_Context == NULL)
		{
#ifdef _WIN32
			closesocket(m_Socket);
#else
			close(m_Socket);
#endif // _WIN32
		}


		if (m_Context == NULL)
		{
			log_error("cant connect redis server : %s", addr);
		}
		return m_Context != NULL;
	}
	return false;
}

void RedisHelper::Close()
{
	if (m_Context)
	{
		redisFree(m_Context);
		m_Context = NULL;
	}
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
		Command(&res, "	EXISTS %s_%s", mapName, key);
	}
	return res.Valid() && res.Count() > 0;
	
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
		Command(&res, "	DEL %s_%s", mapName, key);
	}
	return res.Valid() && res.Count() > 0;
}

bool RedisHelper::SaveValue(int cmd, RedisResponse * res, const char * mapName, const char * key, char * value, size_t len)
{
	if (cmd == REDIS_CMD_HASH)
	{
		Command(res, "HMSET %s %s %b", mapName, key, value, len);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		Command(res, "SET %s_%s %b", mapName, key, value, len);
	}
	
	return res->Valid();
}

bool RedisHelper::GetValue(int cmd, RedisResponse * res, const char * mapName, const char * key)
{
	if (cmd == REDIS_CMD_HASH)
	{
		Command(res, "HGET %s %s", mapName, key);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		Command(res, "GET %s_%s", mapName, key);
	}
	
	return res->Valid();
}

bool RedisHelper::SaveSortValue(RedisResponse * res, const char * mapName, char * value, size_t len, size_t sort)
{
	Command(res, "ZADD %s %d %b", mapName, sort,value, len);
	return res->Valid();
}

bool RedisHelper::GetSortValue(RedisResponse * res, const char * mapName, size_t start, size_t end)
{
	Command(res, "ZRANGE %s %d %d WITHSCORES", mapName,start, end);
	return res->Valid();
}

int RedisHelper::CountSortValue(const char * mapName, size_t start, size_t end)
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
		return res.Count();
	}
	return 0;
}

bool RedisHelper::ExpireKey(int cmd, const char * mapName, const char * key, size_t s)
{
	RedisResponse res;
	if (cmd == REDIS_CMD_HASH)
	{
		Command(&res, "EXPIRE %s %llu", mapName,s);

	}
	else if (cmd == REDIS_CMD_STRING)
	{
		Command(&res, "	EXPIRE %s_%s %llu", mapName, key,s);
	}
	return res.Valid() && res.Count() > 0;
}


void RedisHelper::Command(RedisResponse *res, const char *format, ...)
{
	res->Free();
	va_list ap;
	va_start(ap, format);
	void *reply = NULL;
	reply = redisvCommand(m_Context, format,ap);
	res->m_Reply = (redisReply*)reply;
	va_end(ap);
	res->isChild = false;
	
	
}

RedisResponse::RedisResponse():m_Reply(NULL), isChild(false)
{
}

RedisResponse::~RedisResponse()
{
	Free();
}

bool RedisResponse::Valid()
{
	return m_Reply != NULL;
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

size_t RedisResponse::Count()
{
	if (Valid())return m_Reply->integer;
	return 0;
}

const char * RedisResponse::String()
{
	if (Valid())return m_Reply->str;
	return NULL;
}

size_t RedisResponse::ElementsCount()
{
	if (Valid())return m_Reply->elements;
	return 0;
}

bool RedisResponse::Element(RedisResponse & res, int index)
{
	if (Valid()&&index >= 0 && index < m_Reply->elements)
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
