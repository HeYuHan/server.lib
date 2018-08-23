#pragma once
#ifndef __REDIS_HELPER_H__
#define __REDIS_HELPER_H__

struct redisContext;
struct redisReply;
class RedisHelper;
class RedisResponse
{
	friend class RedisHelper;
public:
	RedisResponse();
	~RedisResponse();
	bool Valid();
	int Type();
	int Length();
	size_t Count();
	const char* String();
	size_t ElementsCount();
	bool Element(RedisResponse &res,int index);
private:
	void Free();
private:
	redisReply* m_Reply;
	bool isChild;
};
enum
{
	REDIS_CMD_HASH = 1, REDIS_CMD_STRING = 2, REDIS_CMD_SORT_STRING = 3
};
class RedisHelper
{
public:
	RedisHelper();
	~RedisHelper();
public:
	bool Connnect(const char* addr);
	void Close();


	bool HaveKey(int cmd,const char* mapName,const char* key);
	bool DelKey(int cmd, const char* mapName, const char* key);
	bool SaveValue(int cmd, RedisResponse *res, const char* mapName, const char* key, char * value, size_t len);
	bool GetValue(int cmd, RedisResponse *res, const char* mapName, const char* key);
	bool SaveSortValue(RedisResponse *res, const char* mapName,char * value, size_t len, size_t score);
	bool GetSortValue(RedisResponse *res, const char* mapName,size_t start,size_t end);
	int CountSortValue(const char* mapName,size_t start, size_t end);
	bool ExpireKey(int cmd, const char* mapName, const char* key, size_t s);

private:
	void Command(RedisResponse *res,const char *format, ...);
private:
	redisContext* m_Context;
};

#endif // !__REDIS_HELPER_H__
