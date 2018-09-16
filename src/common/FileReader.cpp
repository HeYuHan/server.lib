#include "FileReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
//#include<json/reader.h>
#include <string.h>
BEGIN_NS_CORE
using namespace std;
bool ReadText(std::string &ret, const std::string path)
{
	ifstream file_in(path, ios::in);
	if (file_in.is_open())
	{
		std::stringstream buffer;
		buffer << file_in.rdbuf();
		ret = std::string(buffer.str());
		return true;
	}
	return false;
}

bool WriteText(const std::string text, const std::string path)
{
	ofstream file_out(path.c_str());
	bool ret = file_out.is_open();
	if (ret)
	{
		file_out << text.c_str() << endl;
		file_out.close();
	}
	return ret;
}


//bool ReadJson(Json::Value &root, const char* path)
//{
//	ifstream file_in(path,ios::in);
//	bool ret = false;
//	if (!file_in.fail())
//	{
//
//		Json::Reader reader;
//		ret = reader.parse(file_in, root);
//		
//	}
//	file_in.close();
//	return ret;
//}
AsyncFileWriter::AsyncFileWriter():
	mutex(),
	m_ContentFull(),
	m_HaveContent(),
	m_CreateStream(false),
	m_CreateThread(false),
	m_WritePosition(0)
{
	m_ContentFull.Init(&mutex);
	m_HaveContent.Init(&mutex);
}

AsyncFileWriter::~AsyncFileWriter()
{
	Close();
}

bool AsyncFileWriter::Create(const char *path)
{
	m_File.clear();
	//m_File.open(path.c_str(), ios::out| ios::app);
	m_File.open(path, ios::out);
	if (m_File.good())
	{
		m_CreateStream = true;
		if (!m_CreateThread) {
			m_CreateThread = true;
			pthread_create(&m_ThreadID, NULL, WriteThread, this);
		}
	}
	else {
		m_File.close();
	}

	return m_CreateStream;
}

int AsyncFileWriter::PushContent(const char* content)
{
	
	EasyMutexLock lock(mutex);
	int len = strlen(content);
	len = MIN(len, ASYNC_FILE_BUFF_LEN);
	while ((ASYNC_FILE_BUFF_LEN - m_WritePosition)<len)
	{
		m_ContentFull.Wait();
	}
	strcpy(&m_ContentQueue[m_WritePosition], content);
	m_WritePosition += len;
	m_ContentQueue[m_WritePosition] = 0;
	m_HaveContent.Notify();
	return len;
}

int AsyncFileWriter::PushContentLine(const char * content)
{
	EasyMutexLock lock(mutex);
	int len = strlen(content);
	len = MIN(len, ASYNC_FILE_BUFF_LEN-2);
	while ((ASYNC_FILE_BUFF_LEN - m_WritePosition)<len)
	{
		m_ContentFull.Wait();
	}
	strcpy(&m_ContentQueue[m_WritePosition], content);
	m_WritePosition += len+2;
	m_ContentQueue[m_WritePosition-1] = '\n';
	m_ContentQueue[m_WritePosition - 2] = '\r';
	m_ContentQueue[m_WritePosition] = 0;
	m_HaveContent.Notify();
	return len;
}


void AsyncFileWriter::Write()
{
	while (m_CreateThread)
	{
		EasyMutexLock lock(mutex);
		while (m_WritePosition == 0)
		{
			m_HaveContent.Wait();
		}
		m_ContentFull.Notify();

		if (m_WritePosition>0)
		{
			m_File << m_ContentQueue;
			m_File.flush();
		}
		m_WritePosition = 0;
	}
}

void AsyncFileWriter::Close()
{
	if (m_CreateThread)
	{
		pthread_detach(m_ThreadID);
		m_CreateThread = false;
	}
	if (m_CreateStream)
	{
		if (m_WritePosition > 0)
		{
			m_File << m_ContentQueue;
			m_File.flush();
			
		}
		m_File.close();
		m_File.clear();
		m_CreateStream = false;
		m_WritePosition = 0;
	}

}

void* AsyncFileWriter::WriteThread(void * arg)
{
	AsyncFileWriter *t = static_cast<AsyncFileWriter*>(arg);
	if (NULL != t)t->Write();
	return NULL;
}
END_NS_CORE