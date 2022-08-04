#include "ITLog.h"
using namespace log4cplus;
using namespace log4cplus::helpers;

log4cplus::Logger ITLog::m_logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main_log"));
#ifdef _DEBUG
#pragma comment(lib, "../lib/log4cplus/log4cplusUD.lib")
#else
#pragma comment(lib, "../lib/log4cplus/log4cplusU.lib")

#endif
ITLog* ITLog::m_pInstance = NULL;
ITLog::ITLog()
{
	log4cplus::Initializer initializer;
}

ITLog::~ITLog()
{
	log4cplus::Logger::shutdown();
}

ITLog* ITLog::instance()
{
	if (m_pInstance == NULL)
	{
		QMutexLocker lock(&g_instanceMutex);
		if (m_pInstance == NULL) m_pInstance = new ITLog;
	}
	return m_pInstance;
}

bool ITLog::open()
{
	std::wstring strLog = m_strBaseDir + LOG4CPLUS_STRING_TO_TSTRING("\\") + m_strBaseFileName + LOG4CPLUS_STRING_TO_TSTRING(".txt");// LOG4CPLUS_STRING_TO_TSTRING("\\log.txt");
	SharedAppenderPtr _append(new RollingFileAppender((strLog), 10 * 1024 * 1024, 2));
	_append->setName(LOG4CPLUS_TEXT("File Append"));

	SharedObjectPtr<Appender> _append2(new ConsoleAppender());
	_append2->setName(LOG4CPLUS_TEXT("Console Append"));

	std::string pattern = "%D  %m %n";
	_append->setLayout(std::unique_ptr<Layout>(new PatternLayout(LOG4CPLUS_STRING_TO_TSTRING(pattern))));

	std::string pattern2 = "%m %n";
	_append2->setLayout(std::unique_ptr<Layout>(new PatternLayout(LOG4CPLUS_STRING_TO_TSTRING(pattern2))));

	ITLog::m_logger.addAppender(_append);
	ITLog::m_logger.addAppender(_append2);

	ITLog::m_logger.setLogLevel(m_nLogLevel);

	return true;
}

bool ITLog::close()
{
	ITLog::m_logger.removeAllAppenders();
	ITLog::m_logger.shutdown();
	return true;
}

void ITLog::setLogLevel(int nLevel)
{
	m_nLogLevel = getRealLogLevel(nLevel);
}

int ITLog::getRealLogLevel(int nLogLevel)
{
	int nRetLogLevel = INFO_LOG_LEVEL;
	switch (nLogLevel)
	{
	case TLOG_LEVEL_OFF: nRetLogLevel = OFF_LOG_LEVEL; break;
	case TLOG_LEVEL_FATAL: nRetLogLevel = FATAL_LOG_LEVEL; break;
	case TLOG_LEVEL_ERROR: nRetLogLevel = ERROR_LOG_LEVEL; break;
	case TLOG_LEVEL_WARN: nRetLogLevel = WARN_LOG_LEVEL; break;
	case TLOG_LEVEL_INFO: nRetLogLevel = INFO_LOG_LEVEL; break;
	case TLOG_LEVEL_DEBUG: nRetLogLevel = DEBUG_LOG_LEVEL; break;
	case TLOG_LEVEL_TRACE: nRetLogLevel = ALL_LOG_LEVEL; break;
	case TLOG_LEVEL_NOT_SET: nRetLogLevel = NOT_SET_LOG_LEVEL; break;
	default: nRetLogLevel = INFO_LOG_LEVEL; break;
	}
	return nRetLogLevel;
}

void ITLog::setTelnetLogPort(int nPort)
{
	if (nPort <= 0 || nPort >= 65535)
		m_nListenPort = 9999;
	else
		m_nListenPort = nPort;
}
