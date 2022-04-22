#pragma once

// Log.h: interface for the Log class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__B87F71E3_FFAE_4CFA_A528_3F4F2FF7D69E__INCLUDED_)
#define AFX_LOG_H__B87F71E3_FFAE_4CFA_A528_3F4F2FF7D69E__INCLUDED_
#include "iomanip"
#include "log4cplus/configurator.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/helpers/appenderattachableimpl.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/pointer.h"
#include "log4cplus/helpers/property.h"
#include "log4cplus/layout.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/loglevel.h"
#include "log4cplus/ndc.h"
#include "log4cplus/socketappender.h"
#include "log4cplus/tstring.h"
#include <log4cplus/initializer.h>

//#include "const.h"
//#include "common.h"
#include <QMutex>
#include <string>

//日志封装
#define ITLOG_TRACE(p) LOG4CPLUS_TRACE(ITLog::m_logger, p)
#define ITLOG_DEBUG(p) LOG4CPLUS_DEBUG(ITLog::m_logger, p)
#define ITLOG_NOTICE(p) LOG4CPLUS_INFO(ITLog::m_logger, p)
#define ITLOG_WARNING(p) LOG4CPLUS_WARN(ITLog::m_logger, p)
#define ITLOG_FATAL(p) LOG4CPLUS_ERROR(ITLog::m_logger, p)
enum TLOG_LEVEL
{
	TLOG_LEVEL_NOT_SET = -1,
	//	TLOG_LEVEL_ALL = 0,
	TLOG_LEVEL_TRACE = 0,
	TLOG_LEVEL_DEBUG = 1,
	TLOG_LEVEL_INFO = 2,
	TLOG_LEVEL_WARN = 3,
	TLOG_LEVEL_ERROR = 4,
	TLOG_LEVEL_FATAL = 5,
	TLOG_LEVEL_OFF = 6,
};
static QMutex g_instanceMutex;
// 日志控制类，全局共用一个日志
class ITLog
{
public:
	virtual ~ITLog();

	// 打开日志
	bool open();
	bool close();

	// 获得日志实例
	static ITLog* instance();
	void setStrBaseDir(const std::wstring& strDir) { m_strBaseDir = strDir; }
	void setLogFileName(const std::wstring& sName) { m_strBaseFileName = sName; }
	void setLogLevel(int nLevel);
	void setTelnetLogPort(int nPort);
	int getTelnetLogPort() { return m_nListenPort; }
	int getRealLogLevel(int nLogLevel);

	static log4cplus::Logger m_logger;

protected:
	static ITLog* m_pInstance;
	ITLog();

private:
	// log文件路径及名称
	std::wstring m_strBaseDir;
	std::wstring m_strBaseFileName;
	int m_nLogLevel;
	int m_nListenPort;
};
#define LOGGER ITLog::instance()
#endif // !defined(AFX_LOG_H__B87F71E3_FFAE_4CFA_A528_3F4F2FF7D69E__INCLUDED_)