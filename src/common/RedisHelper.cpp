#include "RedisHelper.h"
#include "tools.h"
#include "log.h"
#include <event.h>
BEGIN_NS_CORE
RedisHelper::RedisHelper()
{
}

RedisHelper::~RedisHelper()
{
}
bool RedisHelper::Connect(const char * addr)
{
	sockaddr_in addr_in;
	if (!ParseSockAddr(addr_in, addr))
	{
		log_error("cant parse addr %s", addr);
		return false;
	}
	evutil_socket_t m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (0 == connect(m_Socket, (sockaddr*)&addr_in, sizeof(addr_in)))
	{
		m_Context = redisConnectFd(m_Socket);
		if (m_Context == NULL)
		{
			evutil_closesocket(m_Socket);
		}
		return m_Context != NULL;
	}
	return false;
}
void RedisHelper::Close()
{
	if (m_Context != NULL)
	{
		redisFree(m_Context);
		m_Context = NULL;
	}
}
END_NS_CORE