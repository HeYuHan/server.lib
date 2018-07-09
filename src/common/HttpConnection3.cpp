#include "HttpConnection3.h"
#include <event2/event.h> 
#include <event2/http_struct.h>
#include <event2/buffer.h>
#include <string.h>
#include "Timer.h"
#include "log.h"
HttpManager gHttpManager;
struct RequestPack 
{
	IHttpInterface* handle;
	struct evhttp_connection* connection;
	struct evhttp_request* request;

};
void free_request_pack(RequestPack* p)
{
	if (p) 
	{
		//if (p->url)delete[] p->url;
		delete p;
	}
}
void http_request(struct evhttp_request *req, void *arg)
{

}
void http_request_done(struct evhttp_request *req, void *arg)
{
	/*fprintf(stderr, "< HTTP/1.1 %d\n", evhttp_request_get_response_code(req));
	struct evkeyvalq* headers = evhttp_request_get_input_headers(req);
	struct evkeyval* header = headers->tqh_first;

	while (header)
	{
		fprintf(stderr, "< %s: %s\n", header->key, header->value);
		auto n = header->next;
		header = n.tqe_next;
	}

	fprintf(stderr, "< \n");
	printf("send request ok...\n");
	size_t len = evbuffer_get_length(req->input_buffer);
	unsigned char * str = evbuffer_pullup(req->input_buffer, len);
	char buf[102400] = { 0 };
	memcpy(buf, str, len);
	if (str == NULL)
	{
		printf("len = %d, str == NULL\n", len);
	}
	else
	{
		printf("len = %d, str = %s\n", len, buf);
	}*/
	
	RequestPack * http = static_cast<RequestPack*>(arg);
	if (NULL != http)
	{
		log_info("response %s 1 ","SetInterface");
		gHttpManager.SetInterface(http);
		log_info("response %s 2 ", "SetInterface");
		if (http->handle)http->handle->OnResponse();
		else
		{
#ifdef WIN32
			int state = evhttp_request_get_response_code(req);
			char* content = "";
			size_t len = evbuffer_get_length(req->input_buffer);
			if(len>0)content=(char*)evbuffer_pullup(req->input_buffer, len);
			content[len] = 0;
			log_info("response %d => %s", state,content);
#endif // WIN32
		}
		evhttp_connection_free(http->connection);
		free_request_pack(http);
	}
	
}




bool HttpManager::Request(const char * url, const  char * data, const char* content_type, int port, int flag, IHttpInterface * user_data)
{
	RequestPack *pack = new RequestPack();
	memset(pack, 0, sizeof(RequestPack));
	struct evhttp_uri *uri = evhttp_uri_parse(url);
	const char* host = evhttp_uri_get_host(uri);
	int url_port = evhttp_uri_get_port(uri);
	if (url_port > 0)port = url_port;
	
	struct event_base* base = (NULL == user_data) ? Timer::GetEventBase() : user_data->GetEventBase();
	struct evhttp_connection* connection = evhttp_connection_base_new(base, NULL, host, port);
	struct evhttp_request* req = evhttp_request_new(http_request_done, pack);
	pack->connection = connection;
	pack->request = req;
	pack->handle = user_data;
	evhttp_add_header(req->output_headers, "Host", host);
	char query_path[512] = { 0 };
	int index = 0;
	const char* path = evhttp_uri_get_path(uri);
	if (strlen(path) == 0) {
		strcpy(&query_path[index++], "/");
	}
	else
	{
		strcpy(&query_path[index], path);
		index += strlen(path);
	}
	const char* query = evhttp_uri_get_query(uri);
	if (NULL != query)
	{
		sprintf(&query_path[index], "?%s", query);
	}
	int ret = evhttp_make_request(connection, pack->request, (evhttp_cmd_type)flag, query_path);
	if (flag == EVHTTP_REQ_POST && data != NULL)
	{
		int len = strlen(data);
		evbuffer_add(req->output_buffer, data, len);
		if (!content_type || strlen(content_type) == 0)content_type = "application/x-www-form-urlencoded";
		evhttp_add_header(req->output_headers, "Content-Type", content_type);
	}
	log_info("event make request end : %s", url);
	evhttp_uri_free(uri);
	return ret == 0;
}

bool HttpManager::Get(const char * url, int port, IHttpInterface * user_data)
{
	return Request(url, NULL, NULL,port, EVHTTP_REQ_GET, user_data);
}

bool HttpManager::Get(const char * url, IHttpInterface * user_data)
{
	return Get(url, 80, user_data);
}

bool HttpManager::Post(const char * url, const char * data, const char* content_type, int port, IHttpInterface * user_data)
{
	return Request(url, data, content_type,port, EVHTTP_REQ_POST, user_data);
}

bool HttpManager::Post(const char * url, const char * data, const char* content_type, IHttpInterface * user_data)
{
	return Post(url, data, content_type,80, user_data);
}


void HttpManager::SetInterface(RequestPack * http)
{
	IHttpInterface *handle = http->handle;
	if (handle)
	{
		handle->state = http->request->response_code;
		handle->request = http->request;
		handle->connection = http->connection;
	}
}

IHttpInterface::IHttpInterface():connection(NULL),request(NULL),state(0)
{

}


int IHttpInterface::GetBufferLength()
{
	if (NULL != request && state>0)
	{
		return evbuffer_get_length(request->input_buffer);
	}
	return 0;
}

int IHttpInterface::ReadBuffer(void * data, int size)
{
	if (request != NULL)
	{
		return evbuffer_remove(request->input_buffer, data, size);
	}
	return 0;
}

int IHttpInterface::GetState()
{
	return state;
}

bool IHttpInterface::IsSet()
{
	return this->connection!=NULL && this->request != NULL;
}

event_base * IHttpInterface::GetEventBase()
{

	return Timer::GetEventBase();
}
