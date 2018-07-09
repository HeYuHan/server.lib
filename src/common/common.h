#pragma once
#ifndef __COMMON__H_
#define __COMMON__H_
#define GAME_MSG 254
#define KEEP_ALIVE_MSG 253
#define KEEP_ALIVE_TIME 10.0f
#define BEGIN_NS_CORE namespace Core {
#define END_NS_CORE };
#define USING_NS_CORE using namespace Core;
#define NS_CORE Core

//lib
#if _WIN32
//pthread
#pragma comment(lib,"./../3rd/pthread/lib/x64/pthreadVC2.lib")
//libevent
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"./../3rd/libevent/libevent.lib")
#pragma comment(lib,"./../3rd/libevent/libevent_core.lib")
#pragma comment(lib,"./../3rd/libevent/libevent_extras.lib")
#endif
BEGIN_NS_CORE
typedef long account_id;
typedef long player_id;
typedef unsigned char byte;
typedef byte uint8;
typedef long int64;
typedef unsigned long uint64;

typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
typedef long long llong;
typedef unsigned long long ullong;
END_NS_CORE
#if defined(MACOS)

#define NS_MAP std
#define NS_VECTOR std
#else
#define NS_MAP std::tr1
#define NS_VECTOR std
#endif // defined(MACOS)


#ifdef LINUX
#include <tr1/unordered_map>
#else
#include <unordered_map>
#endif


//#ifdef LINUX
//#if __GNUC__>2
//#define USING_VECOTR using namespace __gnu_cxx;
//#else
//#define USING_VECOTR using namespace stdext;
//#define USING_MAP using namespace std::tr1;
//#endif
//
//#else


//#endif // _WIN32

#define MAX(a,b) (a)>(b)?(a):(b)
#define MIN(a,b) (a)<(b)?(a):(b)

void ThreadSleep(unsigned int s);

#ifdef _WIN32
int gettimeofday(struct timeval *tp, void *tzp);
#endif // _WIN32


#endif // !__COMMON__H_
//#include "BaseServer.h"
//#include "NetworkConnection.h"
//#include "objectpool.h"
//#include "TcpConnection.h"
//#include "ThreadPool.h"
//#include "Timer.h"
//#include "UdpConnection.h"
//#include "UdpListener.h"


