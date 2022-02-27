#include "MLAssistTool.h"
#include <QtWidgets/QApplication>
#include "ITObjectDataMgr.h"
#include "../CGALib/gameinterface.h"
#include "RpcSocket.h"
#include "ITDebugDump.h"
#include "ITSignalProxy.h"
#include "MApplication.h"

extern CGA::CGAInterface* g_CGAInterface;

#ifdef _DEBUG
#pragma comment(lib, "..\\lib\\CGALibd.lib")
#else
#pragma comment(lib, "..\\lib\\CGALib.lib")
#endif // DEBUG
CGA::CGAInterface* g_CGAInterface = NULL;
QtMessageHandler gDefaultHandler = NULL;
void outputMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	static QMutex mutex;
	QMutexLocker lock(&mutex);
	QString text;
	switch (type)
	{
		//如果是debug信息，那么直接打印至应用程序输出，然后退出本函数
	case QtDebugMsg:
	{

		QString sDateMsg = QDateTime::currentDateTime().toString("yyyy:MM:dd hh:mm:ss ");
		g_pSignalProxy->signal_addOneDebugMsg(sDateMsg + msg);
		break;
	}

	//如果是警告，或者是下面的其他消息，则继续执行后面的数据处理
	case QtWarningMsg:
		text = QString("Warning...............................");
		break;

	case QtCriticalMsg:
		text = QString("Critical..............................");
		break;

	case QtFatalMsg:
		text = QString("Fatal.................................");
		break;

	default:
		text = QString("Default...............................");
		break;
	}
	//用系统原来的函数完成原来的功能. 比如输出到调试窗
	if (gDefaultHandler)
	{
		gDefaultHandler(type, context, msg);
	}
}
int main(int argc, char* argv[])
{
	MApplication a(argc, argv);
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
	gDefaultHandler = qInstallMessageHandler(outputMessage);
	g_CGAInterface = CGA::CreateInterface();
	ITObjectDataMgr::getInstance();
	MLAssistTool w;
	w.show();
	return a.exec();
}
