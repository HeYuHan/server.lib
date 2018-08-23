#pragma once
//#include <json/json.h>
//#include <event2/event.h>
#ifdef _WIN32
#include <winsock2.h>
#endif // WIN32


#ifndef __TOOLS_H__
#define __TOOLS_H__

#define FOR_EACH_LIST(_Type_,_List_,_Iter_) \
	std::vector<_Type_*>::iterator iter##_Iter_; \
	for(iter##_Iter_ = _List_.begin(); iter##_Iter_ !=_List_.end(); iter##_Iter_++)

#define FOR_EACH_LIST_STRUCT(__TYPE__,__LIST__,__ITER__) \
	std::vector<__TYPE__>::iterator iter##__ITER__; \
	for( iter##__ITER__ =  __LIST__.begin(); iter##__ITER__ != __LIST__.end(); iter##__ITER__++)

/*
** return a random integer in the interval
** [a, b]
*/
int RandomRange(int a, int b);
//void ParseJsonValue(Json::Value json, const char* key, int &value);
//void ParseJsonValue(Json::Value json, const char* key, float &value);
//void ParseJsonValue(Json::Value json, const char* key, bool &value);
//void ParseJsonValue(Json::Value json, const char* key, char* str, int len);
bool ParseSockAddr(sockaddr_in & addr, const char * str, bool by_name=false);
bool RunAsDaemon();
void CaculateSha1(const char* text, char* out);
float DiffTime(timeval &v1, timeval &v2);
unsigned long long PthreadSelf();
bool IsMainThread();

#endif // !__TOOLS_H__
