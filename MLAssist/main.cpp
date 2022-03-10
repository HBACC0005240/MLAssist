#include "./AStar/AStar.h"
#include "BFSSearch.h"
#include "GPCalc.h"
#include "GameCtrl.h"
#include "ITDebugDump.h"
#include "ITObjectDataMgr.h"
#include "MApplication.h"
#include "MLAssist.h"
#include "MLAssistHttpServer.h"
#include <QDebug>
#include <QTextCodec>
#include <QtWidgets/QApplication>

CGA::CGAInterface *g_CGAInterface = NULL;
QtMessageHandler gDefaultHandler = NULL;
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
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
			g_pGameCtrl->signal_addOneDebugMsg(sDateMsg + msg);
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
void TestAStar();
void TestCompressed();
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

	MApplication a(argc, argv);
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
	MLAssist w;
	a.installEventFilter(&w);
	w.show();
	g_pGameCtrl->RunParseCmd();
	g_pHttpServer->init();
	//QStringList testPetData;
	//testPetData << "水龙蜥" << QString::number(4666)
	//			<< QString::number(1911)
	//			<< QString::number(879)
	//			<< QString::number(899)
	//			<< QString::number(257)
	//			<< QString::number(27)
	//			<< QString::number(373)
	//			<< QString::number(406)
	//			<< QString::number(254)
	//			<< QString::number(232)
	//			<< QString::number(47)
	//			<< QString::number(41);
	//auto pCalcData = g_pGamePetCalc->ParseLine(testPetData);
	//if (pCalcData)
	//{

	//	int grade = pCalcData->lossMin;		   //最少掉档
	//	int lossMinGrade = pCalcData->lossMin; //最少掉档
	//	int lossMaxGrade = pCalcData->lossMax; //最多掉档
	//	qDebug();
	//}

	return a.exec();
}

void TestCompressed()
{
	vector<pair<int, int> > path;
	path.push_back(std::make_pair(1, 1));
	path.push_back(std::make_pair(1, 5));
	path.push_back(std::make_pair(1, 7));
	path.push_back(std::make_pair(1, 10));
	path.push_back(std::make_pair(1, 15));
	path.push_back(std::make_pair(1, 20));
	path.push_back(std::make_pair(1, 25));
	path.push_back(std::make_pair(1, 28));
	path.push_back(std::make_pair(1, 30));
	auto newPath = AStarUtil::compressPath(path);
	//	auto nPath = AStarUtil::expandPath(newPath);
	qDebug() << newPath;
	//qDebug() << nPath;
}
void TestAStar()
{

	AStar findPath(true, true);
	//x是列  y是行
	//竖着的  Y
#define ROWS 11
//横着的  X
#define COLS 12
	AStarGrid grid(COLS, ROWS);
	int map[ROWS][COLS] = {
		{ 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 }
	};
	QString szDebug;
	for (int tmpx = 0; tmpx < ROWS; ++tmpx)
	{
		szDebug.clear();
		for (int tmpy = 0; tmpy < COLS; ++tmpy)
		{
			grid.SetWalkableAt(tmpy, tmpx, !map[tmpx][tmpy]); //灰色 不可行
			szDebug += QString("%1 ").arg(!map[tmpx][tmpy]);
		}
		qDebug() << szDebug;
	}
	QPoint frompos(1, 1);
	QPoint topos(8, 10);

	auto path = findPath.FindPath(frompos.x(), frompos.y(), topos.x(), topos.y(), &grid);
	qDebug() << path.size() << path << "frompos" << frompos << "targetpos" << topos;
}