#include "server.h"
#include <getopt.h>
#include <tools.h>
enum
{
	addr = 0x100,
	channeladdr,
	chataddr,
	infoaddr,
	flag_daemon,
	log_name,
	log_path
};

struct option long_options[] =
{
	{ "addr",required_argument,0,addr },
	{ "channeladdr",required_argument,0,channeladdr },
	{ "chataddr",required_argument,0,chataddr },
	{ "infoaddr",required_argument,0,infoaddr },
	{ "daemon",no_argument,0,flag_daemon },

	{ "log_name",optional_argument,0,log_name },
	{ "log_path",optional_argument,0,log_path }
};
int main(int argc, char **args) {
	

	//uint64 long_uid = 65539;
	//unsigned int channelid = 65538;
	//unsigned int roomid = 65537;
	//unsigned long long m_LongId = (long_uid << 32) | ((channelid & 0xffff) << 16) | (roomid & 0xffff);
	//unsigned int magic = roomid & 0xffff0000;
	//uint m = m_LongId & 0xffff;
	//unsigned int t = m | magic;
	//printf("send token:%llu", m_LongId);
	memset(gServer.m_Addr, 0, sizeof(gServer.m_Addr));
	memset(gServer.m_ChannelAddr, 0, sizeof(gServer.m_ChannelAddr));
	memset(gServer.m_ChatAddr, 0, sizeof(gServer.m_ChatAddr));
	gServer.m_InfoAddrsLength = 0;

	bool as_daemon = false;
	gLogger.logName = "SRS.Proxy";
	char *infoadds[20] = { 0 };
	while (1)
	{
		int option_index = 0;
		int option = getopt_long(argc, args, "", long_options, &option_index);
		if (option <= 0)break;
		switch (option)
		{
		case addr:
			strcpy(gServer.m_Addr, optarg);
			break;
		case channeladdr:
			strcpy(gServer.m_ChannelAddr, optarg);
			break;
		case chataddr:
			strcpy(gServer.m_ChatAddr,optarg);
			break;
		case infoaddr:
			{
				int len = strlen(optarg);
				char *addr = optarg;
				for (int i = 0; i < len; i++)
				{
					if (optarg[i] == '|')
					{
						optarg[i] = 0;
						infoadds[gServer.m_InfoAddrsLength] = addr;
						gServer.m_InfoAddrsLength++;
						addr = optarg + i + 1;
					}
				}
				infoadds[gServer.m_InfoAddrsLength] = addr;
				gServer.m_InfoAddrs = infoadds;
				gServer.m_InfoAddrsLength++;
			}
			break;
		case flag_daemon:
			as_daemon = true;
			break;
		case log_path:
			gLogger.m_LogToFile = true;
			gLogger.filePath = optarg;
			break;
		case log_name:
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
	return gServer.Run();
}
