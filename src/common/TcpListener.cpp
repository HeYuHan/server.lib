#include "TcpListener.h"
#include<string.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#endif // !_WIN32
#include "Timer.h"
#include "tools.h"
#include "log.h"
TcpListener::TcpListener():
	m_Listener(NULL),
	m_Listener2(NULL)
{
}
TcpListener::~TcpListener()
{
	if (m_Listener)
	{
		evconnlistener_free(m_Listener);
		m_Listener = NULL;
	}
	if (m_Listener2)
	{
		event_free(m_Listener2);
	}
}
void ListenEvent2(evutil_socket_t listener, short event, void *arg)
{
	
	//struct event_base *base = (struct event_base *)arg;
	evutil_socket_t fd;
	struct sockaddr_in sin;
	socklen_t slen = sizeof(sin);
	memset(&sin, 0, slen);
	fd = accept(listener, (struct sockaddr *)&sin, &slen);
	if (fd < 0) {
		log_error("cant accept socket %d", fd);
		return;
	}
	//if (fd == 0)
	{
		//int code = evutil_socket_geterror(fd);
		//log_error("accept fd error:%s", evutil_socket_error_to_string(code));
		//return;
	}
	//if (fd > FD_SETSIZE) { //这个if是参考了那个ROT13的例子，貌似是官方的疏漏，从select-based例子里抄过来忘了改
	//	log_error("cant accept socket %d", fd);
	//	return;
	//}
	//log_info("socket listenner event:%d fd:%d", event,fd);

	TcpListener *l = (TcpListener*)arg;
	//sockaddr addr;
	l->OnTcpAccept(fd, (struct sockaddr *)&sin);
}
bool TcpListener::CreateTcpServer(const char * addr, int max_client)
{
	if (!ParseSockAddr(m_ListenAddr, addr, false))return false;
	event_base *base = Timer::GetEventBase();
	
	m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Socket < 0) return -1;
	int r = evutil_make_listen_socket_reuseable(m_Socket);

	r = bind(m_Socket, (struct sockaddr*)&m_ListenAddr, sizeof(m_ListenAddr));
	if (r < 0) return -1;
	r = listen(m_Socket, 65535);
	if (r < 0) return -1;
	if (evutil_make_socket_nonblocking(m_Socket) < 0)
	{
		evutil_closesocket(m_Socket);
		m_Socket = -1;
	}

	m_Listener2 = event_new(base, m_Socket, EV_READ | EV_PERSIST, ListenEvent2,this);
	event_add(m_Listener2, NULL);
	return m_Listener2 != NULL;

	/*m_Listener = evconnlistener_new_bind(Timer::GetEventBase(),
		ListenEvent, this,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		-1, (sockaddr*)&m_ListenAddr, sizeof(m_ListenAddr));
	return m_Listener != NULL;*/
}
bool TcpListener::CreateTcpServer(const char *ip, int port, int max_client)
{
	
	memset(&m_ListenAddr, 0, sizeof(m_ListenAddr));
	m_ListenAddr.sin_family = AF_INET;
	m_ListenAddr.sin_addr.s_addr = inet_addr(ip);
	m_ListenAddr.sin_port = htons(port);
	m_Listener = evconnlistener_new_bind(Timer::GetEventBase(),
		ListenEvent, this,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		-1, (sockaddr*)&m_ListenAddr, sizeof(m_ListenAddr));
	return m_Listener != NULL;
}


void TcpListener::ListenEvent(evconnlistener * listener, evutil_socket_t fd, sockaddr * sock, int socklen, void * arg)
{
	//log_info("socket listenner socklen:%d fd:%d", socklen, fd);
	TcpListener *l = (TcpListener*)arg;
	l->OnTcpAccept(fd, sock);
}