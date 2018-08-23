#include "server.h"
Server gServer;
Server::Server()
{

}
Server::~Server()
{

}
bool Server::Init()
{
	bool ret = BaseServer::Init();

	CreateTcpServer("0.0.0.0:9123", 10);
	m_DBHelper.Connnect("127.0.0.1:6379");
	RedisResponse res;
	m_DBHelper.SaveValue(REDIS_CMD_HASH, &res, "test", "key1", "ffffff", 6);

	return ret;
}
int Server::Run()
{
	Init();
	return BaseServer::Run();
}

void Server::OnTcpAccept(evutil_socket_t socket, sockaddr *)
{
}
