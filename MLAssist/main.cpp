#include "./AStar/AStar.h"
#include "BFSSearch.h"
#include "GPCalc.h"
#include "GameCtrl.h"
#include "ITDebugDump.h"
#include "ITLog.h"
#include "ITNetworkFactory.h"
#include "ITObjectDataMgr.h"
#include "MApplication.h"
#include "MLAssist.h"
#include "MLAssistHttpServer.h"
#include <QDebug>
#include <QTextCodec>
#include <QtWidgets/QApplication>
#include "../lib/vld/vld.h"
#include "../lib/vld/vld_def.h"

//#ifdef _DEBUG
//#pragma comment(lib, "../lib/vld/debug/vld_x86.lib")
//#pragma comment(lib, "../lib/vld/debug/vld.lib")
//#pragma comment(lib, "../lib/vld/debug/libformat.lib")
//#endif

CGA::CGAInterface *g_CGAInterface = NULL;
QtMessageHandler gDefaultHandler = NULL;
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	static QMutex mutex;
	QMutexLocker lock(&mutex);
	QString text;
	switch (type)
	{
		case QtDebugMsg:
		{
			QString sDateMsg = QDateTime::currentDateTime().toString("yyyy:MM:dd hh:mm:ss ");
			g_pGameCtrl->signal_addOneDebugMsg(sDateMsg + msg);
			if (g_pGameCtrl->GetCreateLog())
				ITLOG_DEBUG(msg.toStdString().c_str());
			break;
		}
		//case QtWarningMsg:
		//{
		//	ITLOG_WARNING(msg.toStdString().c_str());
		//	break;
		//}
		//case QtCriticalMsg: ITLOG_NOTICE(msg.toStdString().c_str()); break;
		//case QtFatalMsg: ITLOG_FATAL(msg.toStdString().c_str()); break;
		//default: ITLOG_NOTICE(msg.toStdString().c_str()); break;
		default: break;
	}
	//用系统原来的函数完成原来的功能. 比如输出到调试窗
	if (gDefaultHandler)
	{
		gDefaultHandler(type, context, msg);
	}
}
int main(int argc, char *argv[])
{
	//高分辨率适配
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	qreal cx = GetSystemMetrics(SM_CXSCREEN);
	qreal scale = cx / 1920;
	if (scale < 1)
	{
		scale = 1;
	}
	qputenv("QT_SCALE_FACTOR", QString::number(scale).toLatin1());
	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()) + QCoreApplication::applicationPid());
	MApplication a(argc, argv);
	qputenv("CGA_DIR_PATH", QCoreApplication::applicationDirPath().toLocal8Bit());
	qputenv("CGA_DIR_PATH_UTF8", QCoreApplication::applicationDirPath().toUtf8());
	qputenv("CGA_GAME_PORT", "");
	qputenv("CGA_GUI_PORT", "");

	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
	qDebug() << "线程池最大线程个数：" << QThreadPool::globalInstance()->maxThreadCount();
	//当前活动的线程个数
	qDebug() << "当前活动的线程个数：" << QThreadPool::globalInstance()->activeThreadCount();
	//设置最大线程数
	QThreadPool::globalInstance()->setMaxThreadCount(100);
	qDebug() << "线程池最大线程个数：" << QThreadPool::globalInstance()->maxThreadCount();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
#else
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif
	gDefaultHandler = qInstallMessageHandler(outputMessage);
	a.setWindowIcon(QIcon(":/MLAssist/Resources/logo.ico"));
	g_CGAInterface = CGA::CreateInterface();
	QTranslator tsQT;
	tsQT.load(QApplication::applicationDirPath() + "/qt_zh_CN.qm");
	a.installTranslator(&tsQT);
	QTranslator translator;
	if (translator.load(QApplication::applicationDirPath() + "/lang.qm"))
		a.installTranslator(&translator);
	g_pGameCtrl->InitCmdParse(); //先于ITObjectDataMgr调用 否则线程中调用后 信号会失效
	ITObjectDataMgr::getInstance().init();
	ITNetworkFactory::getInstace();
	MLAssist w;
	a.setWindowInstance(&w);
	a.installEventFilter(&w);
	w.show();
	g_pGameCtrl->RunParseCmd();
	g_pHttpServer->init();
	int ret= a.exec();
	//SafeDelete(g_CGAInterface);
	return ret;
}