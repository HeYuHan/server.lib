#pragma once
#ifndef __REDIS_HELPER_H__
#define __REDIS_HELPER_H__
#include "log.h"
#include <hiredis.h>
#include <common.h>
BEGIN_NS_CORE
class RedisHelper
{
public:
	RedisHelper();
	~RedisHelper();
public:
	bool Connect(const char* addr);
	void Close();

private:
	redisContext *m_Context;
};






END_NS_CORE

#endif // !__REDIS_HELPER_H__
