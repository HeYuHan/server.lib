#include "rpcconnection.h"
#include <log.h>
#include <tools.h>
#include <ThreadEventPool.h>
#include "rpcrequest.h"
#include <ClientData.pb.h>
#include "ClientData.pb.h"
#include "server.h"
RPCRequestSig RPC_ALL_REQUEST_INFO[RPC_SIG_COUNT] = { 0 };

RPCClient::RPCClient() :m_RPC(NULL), m_CurrentRequest(NULL)
{
}

RPCClient::~RPCClient()
{
}

bool RPCClient::ThreadSafe()
{
	return true;
}

void RPCClient::OnRevcMessage(bool parse)
{
	NetworkStream::OnRevcMessage(parse);
}
void RPCClient::ParseMessage()
{
	NetworkStream::ParseMessage();
}

uint RPCClient::GetUid()
{
	return uid;
}

void RPCClient::OnDisconnect()
{
	if (m_CurrentRequest)
	{
		m_CurrentRequest->m_Send = false;
		if (m_RPC)
		{
			m_RPC->AddRequest(m_CurrentRequest);
		}
		/*delete m_CurrentRequest;*/
		m_CurrentRequest = NULL;
	}
	if(m_RPC && connection)this->m_RPC->AddLostAddr(connection->GetSocket());
	m_RPC->m_CachedClient.Free(uid);
}

void RPCClient::OnConnected()
{
	NetworkStream::Reset();
	m_ReuqestIndex = 0;
	//Proto::Protocol::Header header;
	//Proto::Message::PlayerCreateRequest request;
	//request.set_userid(8);
	//request.set_name("1457");
	//char msg[256] = {0};
	//request.SerializeToArray(msg, 256);
	//gServer.ClientRpcProxy(NULL, "PlayerCreate", 1, msg, request.ByteSize());
	//RPC_CALL(Proto::Message::PlayerCreate, RPC_SIG_CLIENT_PROXY, header, request, NULL, 0);
}

void RPCClient::OnReconnected(SocketPoolClinet*)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void RPCClient::Init()
{
	m_CurrentRequest = NULL;
	m_RPC = NULL;

}

bool RPCClient::IsConnect()
{
	return connection != NULL;
}

void RPCClient::OnWrite()
{
	//EasyMutexLock l(m_Lock);
	if (m_CurrentRequest)
	{
		timeval current;
		evutil_gettimeofday(&current, NULL);
		float timeout = DiffTime(current, m_CurrentRequest->m_RequestTimeBegin);
		//log_info("rpc request %s time out %f", m_CurrentRequest->m_RequestHeader.methodname().c_str(), timeout);
		if (timeout > 20)
		{
			log_error("rpc reqeust is time out %s", m_CurrentRequest->m_RequestHeader.methodname().c_str());
			m_CurrentRequest->m_Processed = true;
			if (m_CurrentRequest->m_IsProxy)
			{
				m_CurrentRequest->m_ResponseProxy = NULL;
				m_CurrentRequest->m_ResponseProxyLength = 0;
				m_CurrentRequest->m_ResponseHeader.set_code(Proto::Message::ResponseCode::TIMEOUT);
				if (RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_CallBack)
				{
					RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_CallBack(m_CurrentRequest);
				}
			}
			else
			{
				m_CurrentRequest->m_ResponseHeader.set_code(Proto::Message::ResponseCode::TIMEOUT);
				if (RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_CallBack)
				{
					RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_CallBack(m_CurrentRequest);
				}
			}
			delete m_CurrentRequest;
			m_CurrentRequest = NULL;
		}
		else
		{
			return;
		}
		
	}
	
	if (!m_CurrentRequest)
	{
		m_CurrentRequest = m_RPC->GetRequest();
		if (!m_CurrentRequest)
		{
			m_ReuqestIndex = 0;
		}
	}

	if (m_CurrentRequest && !m_CurrentRequest->m_Send)
	{
		m_CurrentRequest->m_RequestIndex = m_ReuqestIndex++;
		evutil_gettimeofday(&m_CurrentRequest->m_RequestTimeBegin, NULL);
		BeginWrite();
		if (m_CurrentRequest)
		{
			
			//write header
			{
				int header_size = m_CurrentRequest->m_RequestHeader.ByteSize();
				WriteInt(header_size);
				int empty_size = write_buff_end - write_end;
				if (empty_size < header_size)
				{
					log_error("buffer size too small header size:%d", header_size);
				}
				else
				{
					m_CurrentRequest->m_RequestHeader.SerializeToArray(write_end, empty_size);
					write_end += header_size;
				}
			}
			//write msg
			{
				if (!m_CurrentRequest->m_IsProxy)
				{
					google::protobuf::Message *msg = m_CurrentRequest->m_Request;
					int size = msg->ByteSize();
					int empty_size = write_buff_end - write_end;
					if (empty_size < size)
					{
						log_error("buffer size too small msg size:%d", size);
					}
					else
					{
						msg->SerializeToArray(write_end, empty_size);
						write_end += size;
					}
				}
				else
				{
					WriteData(m_CurrentRequest->m_RequsetProxy, m_CurrentRequest->m_RequestProxyLength);
				}
			}
			
		}
		EndWrite();
		m_CurrentRequest->m_Send = true;
		
		log_info("rpc client %d wait response %s index %d", uid,m_CurrentRequest->m_RequestHeader.methodname().c_str(),m_CurrentRequest->m_RequestIndex);
	}
	
}

Core::Event * RPCClient::GetEvent()
{
	return m_RPC->m_Event;
}

void RPCClient::OnMessage()
{
	
	if (!m_CurrentRequest || m_CurrentRequest->m_RequestIndex != (m_ReuqestIndex - 1))
	{
		log_error("%s","request is null or request index is error(maybe time out,request removed)");
		//OnWrite();
		return;
	}
	int head_len = 0;
	ReadInt(head_len);
	int un_read_len = read_end - read_position;
	if (head_len > un_read_len)
	{
		log_error("head length is too long %d unread %d method name:", head_len, un_read_len, m_CurrentRequest->m_RequestHeader.methodname().c_str());
	}
	else
	{
		bool parse_head = m_CurrentRequest->m_ResponseHeader.ParseFromArray(read_position, head_len);
		if (!parse_head || m_CurrentRequest->m_ResponseHeader.rpcid()!=m_CurrentRequest->m_RequestHeader.rpcid())
		{
			log_error("response heard cant match requset:%s res:%s", m_CurrentRequest->m_RequestHeader.methodname().c_str(), m_CurrentRequest->m_ResponseHeader.methodname().c_str());
		}
		else 
		{
			timeval current;
			gettimeofday(&current, NULL);
			float timeout = DiffTime(current, m_CurrentRequest->m_RequestTimeBegin);
			log_info("rpc response %d method name %s time %f code %d", uid,m_CurrentRequest->m_ResponseHeader.methodname().c_str(), timeout,m_CurrentRequest->m_ResponseHeader.code());
			if (m_CurrentRequest->m_IsProxy)
			{
				m_CurrentRequest->m_ResponseProxy = read_position + head_len;
				m_CurrentRequest->m_ResponseProxyLength = un_read_len - head_len;
				if (RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_CallBack)
				{
					RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_CallBack(m_CurrentRequest);
				}
			}
			else if (m_CurrentRequest->m_Response)
			{
				if (m_CurrentRequest->m_ResponseHeader.code() == 0)
				{
					bool parse_content = m_CurrentRequest->m_Response->ParseFromArray(read_position + head_len, un_read_len - head_len);
					if (!parse_content)
					{
						log_error("cant parse rpc resopnse message %s", m_CurrentRequest->m_RequestHeader.methodname().c_str());
					}
				}
				if (RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_CallBack)
				{
					RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_CallBack(m_CurrentRequest);
				}
			}

			
		}

	}
	m_CurrentRequest->m_Processed = true;
	delete m_CurrentRequest;
	m_CurrentRequest = NULL;
	/*ushort sig = RPC_SIG_NONE;
	ReadUShort(sig);
	if (sig == m_CurrentRequest->m_SIG)
	{
		if (RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_CallBack)
		{
			if (m_CurrentRequest->m_RecvProto)
			{
				google::protobuf::Message *msg = static_cast<google::protobuf::Message*>(m_CurrentRequest->m_RecvProto);
				int size = read_end - read_position;
				bool b = msg->ParseFromArray(read_position, size);
				if (!b)
				{
					log_error("cant parse proto buff size:%d,method:%s", size, RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_MethodName);
				}
				read_position += size;
				RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_CallBack(m_CurrentRequest->m_SIG, m_CurrentRequest->m_UserData, m_CurrentRequest->m_RecvProto);
			}
			else
			{
				RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_CallBack(m_CurrentRequest->m_SIG, m_CurrentRequest->m_UserData, NULL);
			}
			
		}
	}
	else
	{
		log_error("rpc call sig error return sig:%d origin sig:%d method:%s", sig, m_CurrentRequest->m_SIG, RPC_ALL_REQUEST_INFO[m_CurrentRequest->m_SIG].m_MethodName);
	}*/

	

}

void UpdateRPCConnection(float t, void * arg)
{
	RPCConection *c = static_cast<RPCConection*>(arg);
	if (c)c->Update(t);
}

RPCConection::RPCConection():m_AllInfoServerAddr(NULL),m_AllInfoServerLength(0)
{
}

RPCConection::~RPCConection()
{
	if (m_AllInfoServerAddr)
	{
		delete[] m_AllInfoServerAddr;
		m_AllInfoServerAddr = NULL;
		m_AllInfoServerLength = 0;
	}
}

void RPCConection::OnAcceptPoolClinet(SocketPoolClinet* c, void *user_data)
{
	RPCConection * rpc = static_cast<RPCConection*>(user_data);
	if (!rpc)
	{
		c->Disconnect();
		log_error("cant cast user data to RPCConnection %p", user_data);
		return;
	}
	RPCClient *client = rpc->m_CachedClient.Allocate();
	if (!client)
	{
		c->Disconnect();
		log_error("cant allocate more rpclient pool size:%d", rpc->m_CachedClient.Size());
		return;
	}
	client->Init();
	client->m_RPC = rpc;
	client->uid = c->uid;
	c->m_Type = TCP_SOCKET;
	client->connection = c;
	c->m_Handle = client;
}

bool RPCConection::Connect(int addrc, char ** addrs)
{
	
	if (!m_SocketPool.Init(addrc, false, RPCConection::OnAcceptPoolClinet, true,this))return false;
	if (!m_CachedClient.Initialize(addrc))return false;
	m_AllInfoServerAddr = new ConnectResult[addrc];
	if (m_AllInfoServerAddr == NULL)return false;
	memset(m_AllInfoServerAddr, 0, sizeof(ConnectResult)*addrc);
	m_AllInfoServerLength = addrc;
	for (int i = 0; i < addrc; i++)
	{
		m_AllInfoServerAddr[i].user_set = 0;
		strcpy(m_AllInfoServerAddr[i].addr_ip_port, addrs[i]);
		gConnectHelper.Add(&m_AllInfoServerAddr[i]);
		/*sockaddr_in sock_in;
		ParseSockAddr(sock_in, addrs[i]);
		bool ret = m_SocketPool.Connect((sockaddr*)&sock_in,sizeof(sock_in));
		if (!ret)
		{
			log_error("cant connect rpc server:%s",addrs[i]);
			AddLostAddr(sock_in);
		}*/
	}
	m_ReconectLostAddrTime = 100;
	m_Event = gEventPool.Get();
	m_UpdateTimer.Init(0.01f, UpdateRPCConnection, this, true);
	m_UpdateTimer.Begin(m_Event);
	return true;
}

void RPCConection::Update(float t)
{

	m_ReconectLostAddrTime += t;
	if (m_ReconectLostAddrTime > 5)
	{
		m_ReconectLostAddrTime = 0;
		for (int i = 0; i < m_AllInfoServerLength; i++)
		{
			ConnectResult *result = &m_AllInfoServerAddr[i];
			if (result->Connected()&&!result->user_set)
			{
				log_info("connected rpc server:%s", result->addr_ip_port);
				result->user_set = 1;
				m_SocketPool.AcceptClient(result->result_fd, (sockaddr*)&result->addr_sock);
			}
			if (!result->Connected() && !result->Connecting())
			{
				log_error("cant connect rpc server:%s", result->addr_ip_port);
				result->user_set = 0;
				gConnectHelper.Add(result);
			}
		}
		//m_Lock.Lock();
		//std::queue<sockaddr_in> temp;
		//while (!m_LostAddr.empty())
		//{
		//	sockaddr_in sock_in = m_LostAddr.front();
		//	char addr[128] = { 0 };
		//	sprintf(addr,"%s:%d",inet_ntoa(sock_in.sin_addr), ntohs(sock_in.sin_port));
		//	log_info("begin reconnect rpc server:%s", addr);
		//	bool ret = gSocketPool.Connect((sockaddr*)&sock_in, sizeof(sock_in));
		//	if (!ret)
		//	{
		//		log_error("cant connect rpc server:%s", addr);
		//		temp.push(sock_in);
		//	}
		//	m_LostAddr.pop();
		//	
		//}
		//m_LostAddr = temp;
		//m_Lock.Unlock();
	}
	RPCClient *client = m_CachedClient.Begin();
	for (uint i = 0; i < m_CachedClient.Size(); i++)
	{
		client = client + i;
		if (client->IsConnect())
		{
			client->OnWrite();
		}
	}
}

//void RPCConection::Call(RPCRequestPack<google::protobuf::Message, google::protobuf::Message>* pack)
//{
//	EasyMutexLock l(m_Lock);
//	pack->m_Processed = false;
//	pack->m_Send = false;
//	pack->m_RequestHeader.set_methodname(RPC_ALL_REQUEST_INFO[pack->m_SIG].m_MethodName);
//	pack->m_RequestHeader.set_rpcid(pack->m_SIG);
//	m_RequestQueue.push(pack);
//}

//void RPCConection::Call(ushort sig, void *send_proto, void *recv_proto, void **user_data)
//{
//	void *arg[1] = { user_data };
//	Call(sig, send_proto, recv_proto,arg, 1);
//}
//
//void RPCConection::Call(ushort sig, void *send_proto, void *recv_proto, void **user_data, int data_len)
//{
//	RPCRequestInfo *info = new RPCRequestInfo();
//	memset(info, 0, sizeof(RPCRequestInfo));
//	info->m_SIG = sig;
//	info->m_Processed = false;
//	info->m_SendProto = send_proto;
//	info->m_RecvProto = recv_proto;
//	info->m_Send = false;
//	info->m_UserDataLen = data_len;
//	for (int i = 0; i < data_len; i++)
//	{
//		info->m_UserData[i] = user_data[i];
//	}
//	EasyMutexLock l(m_Lock);
//	m_RequestQueue.push(info);
//}

void RPCConection::AddLostAddr(int fd)
{
	if (fd <= 0)return;
	for (int i = 0; i < m_AllInfoServerLength; i++)
	{
		if (m_AllInfoServerAddr[i].result_fd == fd)
		{
			log_error("disconnect rpc server:%s", m_AllInfoServerAddr[i].addr_ip_port);
			gConnectHelper.Add(&m_AllInfoServerAddr[i]);
			m_AllInfoServerAddr[i].user_set = 0;
		}
	}
}

RPCRequestPack<google::protobuf::Message, google::protobuf::Message> * RPCConection::GetRequest()
{
	EasyMutexLock l(m_Lock);
	if (m_RequestQueue.empty())return NULL;
	RPCRequestPack<google::protobuf::Message, google::protobuf::Message> *info = m_RequestQueue.front();
	m_RequestQueue.pop();
	return info;
}

void RPCConection::AddRequest(RPCRequestPack<google::protobuf::Message, google::protobuf::Message> * info)
{
	m_Lock.Lock();
	info->m_Processed = false;
	info->m_Send = false;
	m_RequestQueue.push(info);
	m_Lock.Unlock();
	RPCClient* client = m_CachedClient.Begin();
	for (uint i = 0; i < m_CachedClient.Size(); i++)
	{
		RPCClient* temp = client + i;
		if (temp->IsConnect())
		{
			temp->OnWrite();
		}
	}
}
