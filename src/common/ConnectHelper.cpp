#include "ConnectHelper.h"
#include "tools.h"
#include <log.h>
BEGIN_NS_CORE
ConnectHelper gConnectHelper;
ConnectResult::ConnectResult():connected(false),connecting(false)
{
}
bool ConnectResult::Connecting()
{
	return connecting;
}
bool ConnectResult::Connected()
{
	return connected;
}
ConnectTask::ConnectTask():helper(NULL)
{

}
void ConnectTask::Process()
{
	while (helper)
	{
		ConnectResult *result = helper->GetWaitAddr();
		if (result != NULL)
		{
			ParseSockAddr(result->addr_sock, result->addr_ip_port);
			result->result_fd = socket(AF_INET, SOCK_STREAM, 0);
			if (0 == connect(result->result_fd, (sockaddr*)&result->addr_sock, sizeof(result->addr_sock)))
			{
				result->connected = true;
				result->connecting = false;
			}
			else
			{
				//log_error("cant connect server %s", result->addr_ip_port);
				evutil_closesocket(result->result_fd);
				result->result_fd = -1;
				result->connecting = false;
				//if (helper)helper->Add(result);
			}
		}
	}
}

ConnectHelper::ConnectHelper():m_Inited(false)
{
}

ConnectHelper::~ConnectHelper()
{
	for (int i = 0; i < 5; i++)
	{
		m_Tasks[i].helper = NULL;
	}
	m_ThreadPool.Stop();
}
void ConnectHelper::Add(ConnectResult *result)
{
	if (result->connecting)return;
	result->connected = false;
	result->connecting = true;
	EasyMutexLock l(m_Lock);
	if (!m_Inited)Init();
	m_WaitConnectList.push(result);
}
void ConnectHelper::Init()
{
	if (m_Inited)return;
	m_ThreadPool.Start(5, 5);
	for (int i = 0; i < 5; i++)
	{
		m_Tasks[i].helper = this;
		m_ThreadPool.AddTask(&m_Tasks[i]);
	}
	
}
ConnectResult * ConnectHelper::GetWaitAddr()
{
	EasyMutexLock l(m_Lock);
	if (m_WaitConnectList.empty())return NULL;
	ConnectResult *result = m_WaitConnectList.front();
	m_WaitConnectList.pop();
	return result;
}
END_NS_CORE


