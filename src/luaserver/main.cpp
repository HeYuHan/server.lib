#include <stdio.h>

#include <LuaEngine.h>
#include <curl/curl.h>

using namespace luabridge;
using namespace Core;

#include <log.h>

#include <luaregister.h>
#include <getopt.h>
#include <tools.h>

enum
{
	script = 0x100,
	flag_logname,
	flag_logpath,
	flag_daemon,
	flag_v8
};
struct option long_options[] =
{
	{ "script",required_argument,0,script },
	{ "log_name",1,0,flag_logname },
	{ "log_path",1,0,flag_logpath },
	{ "daemon",0,0,flag_daemon },
};
int main(int argc, char **args)
{
	gLogger.logName = "NTCP";
	char script_name[256] = { 0 };
	bool as_daemon = false;
	while (1)
	{
		int option_index = 0;
		int option = getopt_long(argc, args, "", long_options, &option_index);
		if (option <= 0)break;
		switch (option)
		{
		case script:
			strcpy(script_name, optarg);
			break;
		case flag_daemon:
			as_daemon = true;
			break;
		case flag_logpath:
			gLogger.m_LogToFile = true;
			gLogger.filePath=optarg;
			break;
		case flag_logname:
			gLogger.logName = optarg;
			break;
		case '?':
			return 1;
			break;
		default:
			break;
		}
	}
	if (as_daemon && !RunAsDaemon())
	{
		log_error("%s", "run as daemon error!");
		return -1;
	}
	LuaEngine::GetInstance()->RegisterModel(RegAllNative);
	LuaEngine::GetInstance()->Start();
	LuaEngine::GetInstance()->LuaSearchPath("path", "./?.lua");
	LuaEngine::GetInstance()->LuaSearchPath("path", "../?.lua");
	LuaEngine::GetInstance()->LuaSearchPath("path", "../src/?.lua");
	LuaEngine::GetInstance()->DoFile(script_name);
	return 1;
}