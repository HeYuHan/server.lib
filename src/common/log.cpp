#include "log.h"
#include <memory.h>
Logger gLogger;
Logger::Logger():
	logName("SRS"),
	filePath("./log/srs.log"),
	m_LogToConsole(true),
	m_LogToFile(false),
	logger(NULL)
{
}
Logger::~Logger()
{
	/*if(NULL != logger)logger->shutdown();
	logger = NULL;*/
}

void Logger::Init()
{
	PatternLayout *layout = new PatternLayout();
	layout->setConversionPattern("%d:%c|%p:%m%n");
	logger = &Category::getRoot().getInstance(logName);
	if (m_LogToFile)
	{
		RollingFileAppender *fileAppender = new RollingFileAppender(logName, filePath);
		fileAppender->setLayout(layout);
		logger->addAppender(fileAppender);
	}

	if (m_LogToConsole)
	{
		OstreamAppender *consoleAppender = new OstreamAppender(logName, &std::cout);
		consoleAppender->setLayout(layout);
		logger->addAppender(consoleAppender);
	}


	logger->setPriority(Priority::DEBUG);
}

Category & Logger::GetLogger()
{
	if (NULL == logger)Init();
	return *logger;
}
