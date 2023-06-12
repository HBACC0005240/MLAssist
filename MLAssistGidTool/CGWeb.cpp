#include <QJsonDocument>
#include "CGWeb.h"
#include <QWebEnginePage>
#include <QWebEngineScript>
#include <QFile>
#include <QUrlQuery>
#include <QTimer>
#include <QNetworkReply>
#include <QTextCodec>
#include <QNetworkCookieJar>
#include <windows.h>
#include <QMessageBox>
#include <QSettings>
CGWeb::CGWeb(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	progress = 0;

	m_gameTypeForUrl.insert(13,QStringList() << "https://www.polchina.com.cn/register/?type=creategid&game=ItemSales");
	m_gameTypeForUrl.insert(14,QStringList() << "https://www.polchina.com.cn/register/?type=creategid&game=ItemSales");
	m_gameTypeForUrl.insert(1,QStringList() << "https://www.polchina.com.cn/register/?type=creategid&game=PUK2");
	m_gameTypeForUrl.insert(23,QStringList() << "https://www.polchina.com.cn/register/?type=creategid&game=cg_old");
	m_gameTypeForUrl.insert(24,QStringList() << "https://www.polchina.com.cn/register/?type=creategid&game=cg_old");
	ui.comboBox_type->clear();
	QMap<QString, int> gameTypeMap;
	gameTypeMap.insert("道具电信", 13);
	gameTypeMap.insert("道具网通", 14);
	gameTypeMap.insert("时长", 1);
	gameTypeMap.insert("怀旧牧羊双子", 23);
	gameTypeMap.insert("怀旧金牛", 24);
	for (auto it= gameTypeMap.begin();it!= gameTypeMap.end();++it)
	{
		ui.comboBox_type->addItem(it.key(),it.value());
	}
	ui.comboBox_type->setCurrentIndex(ui.comboBox_type->findData(13));
	QString iniPath = QCoreApplication::applicationDirPath() + "/config.ini";
	QSettings iniFile(iniPath, QSettings::IniFormat);

	m_precisionNum = iniFile.value("game/precision", "").toInt();
	if (m_precisionNum < 2)
		m_precisionNum = 2;


	//qt jquery js: Uncaught TypeError: Failed to execute 'evaluate' on 'Document'  js报错 1.3.1  替换高版本js后解决
	QFile file;
	QString jsPath = QCoreApplication::applicationDirPath() + "/resources/jquery.min.js";
	//file.setFileName(":/jquery.min.js");
	file.setFileName(jsPath);
	file.open(QIODevice::ReadOnly);
	jQuery = file.readAll();
	jQuery.append("\nvar qt = { 'jQuery': jQuery.noConflict(true) };");
	file.close();
	ui.webEngineView->load(QUrl("https://www.polchina.com.cn/user/login.php"));
	connect(ui.webEngineView, &QWebEngineView::loadFinished, this, &CGWeb::finishLoading);
	connect(ui.webEngineView, &QWebEngineView::urlChanged, this, &CGWeb::onUrlChanged);
	connect(&m_createGidTimer, SIGNAL(timeout()), this, SLOT(onCheckCreateGid()));

}

CGWeb::~CGWeb()
{
}
void CGWeb::finishLoading(bool)
{
	progress = 100;
	//注入js 
	qDebug() << "finishLoading" << ui.webEngineView->page()->url().url();
	ui.webEngineView->page()->runJavaScript(jQuery, [](const QVariant& v) { qDebug() << v.toString(); });
	//rotateImages(true);
	QString sCurUrl = ui.webEngineView->page()->url().url();

	QString sCretaeGidUrl = m_gameTypeForUrl.value(m_nGameType)[0];//创建游戏ID界面

	if (sCurUrl == "https://www.polchina.com.cn/user/bodymember.php" && m_bCreateGid)//进去后界面，跳转创建用户id界面
	{
		ui.webEngineView->load(QUrl(sCretaeGidUrl));	
	}
	else if (sCurUrl.contains("https://www.polchina.com.cn/register/?type=creategid_ok&gid") && m_bCreateGid)
	{
		//https://www.polchina.com.cn/register/?type=creategid_ok&gid=123456789&region=23  前置一样 

		m_lastCreateGidTime.restart();
		m_nBeginNum++;
		qDebug() << "GidNum:" << m_nBeginNum;
		ui.webEngineView->load(QUrl(sCretaeGidUrl));
	}
	else if (sCurUrl == sCretaeGidUrl&& m_bCreateGid)	//"https://www.polchina.com.cn/register/?type=creategid&game=ItemSales" 
	{
		m_lastCreateGidTime.restart();
		Sleep(1000);	//休息2秒 防止过快 503 貌似 多块都一样
		QString sGid = ui.lineEdit_gid->text();
		QString sNewGidNum = QString("%1").arg(m_nBeginNum, m_precisionNum, 10, QLatin1Char('0'));
		sGid += sNewGidNum;
		qDebug() << "CreateNewGidNum:" << sGid;
		QString sIDName = "gid_13";
		QString sClickCode = "qt.jQuery('.b form:first').submit()";
		switch (m_nGameType)
		{
		case 1:		//时长
		{	
			sIDName = "gid_1";
			sClickCode = "qt.jQuery('.b form:first').submit()";
			break;
		}	
		case 13:	//道具电信
		{
			sIDName = "gid_13";
			sClickCode = "qt.jQuery('.b form:first').submit()";
			break;
		}
		case 14:	//道具网通
		{	
			sIDName = "gid_14";
			sClickCode = "qt.jQuery('.b form:eq(1)').submit()";
			break;
		}case 23:	//怀旧牧羊双子
		{	
			sIDName = "gid_23";
			sClickCode = "qt.jQuery('.b form:first').submit()";
			break;
		}case 24:	//怀旧金牛
		{	
			sIDName = "gid_24";
			sClickCode = "qt.jQuery('.b form:eq(1)').submit()";
			break;
		}
		default:
			break;
		}	
		QString sNewGid = QString("qt.jQuery('#%1').val('%2')\n").arg(sIDName).arg(sGid);
		QString sScriptCode;
		sScriptCode += sNewGid;
		sScriptCode += sClickCode;
		ui.webEngineView->page()->runJavaScript(sScriptCode, [](const QVariant& v) { qDebug() << v.toString(); });
	}
	else if (sCurUrl.contains("在每一个通行证帐号下最多能创建32个游戏帐号"))
	{
		m_lastCreateGidTime.restart();
		m_bCreateGid = false;
		m_createGidTimer.stop();
		QMessageBox::information(this, "提示：", "创建账号完成", "确定");
	}
}
void CGWeb::rotateImages(bool invert)
{
	QString code;

	if (invert)
		code = QStringLiteral("qt.jQuery('img').each( function () { qt.jQuery(this).css('transition', 'transform 2s'); qt.jQuery(this).css('transform', 'rotate(180deg)') } )");
	else
		code = QStringLiteral("qt.jQuery('img').each( function () { qt.jQuery(this).css('transition', 'transform 2s'); qt.jQuery(this).css('transform', 'rotate(0deg)') } )");
	ui.webEngineView->page()->runJavaScript(code, [](const QVariant& v) { qDebug() << v.toString(); });
}

void CGWeb::judgePageUrl()
{
	if (ui.webEngineView->page()->url().url() == "https://www.polchina.com.cn/user/bodymember.php")//进去后界面，跳转创建用户id界面
	{
		ui.webEngineView->load(QUrl("https://www.polchina.com.cn/register/?type=creategid&amp;game=0"));	//创建id
		ui.webEngineView->load(QUrl("https://www.polchina.com.cn/register/?type=creategid&game=ItemSales"));	//道具服务器
		ui.webEngineView->load(QUrl("https://www.polchina.com.cn/user/getgid.php?product_id=4&region_id=13"));	//查询道具二下所有游戏id
		//ui.webEngineView->load(QUrl("https://www.polchina.com.cn/user/getgid.php?product_id=4&region_id=13"));	//查询道具二下所有游戏id 界面有选项可以切换 后续可以考虑做个查询
		QString sGid = "myworld";
		QString sUserCode = QString("qt.jQuery('#gid_13').val('%1')\n").arg(sGid);	//电信gid_13  网通gid_14
		//QString sUserCode = QString("qt.jQuery('#gid_14').val('%1')\n").arg(sGid);
	/*	QString sPwdCode = QString("qt.jQuery('#passwd').val('%1')\n").arg(sPwd);
		QString sLoginCode = QString("qt.jQuery('#finish').click()\n");*/
	}
}


void CGWeb::on_pushButton_2_clicked()
{
	m_bCreateGid = true;
	m_bCreateAccount = false;
	QString sCretaeGidUrl = m_gameTypeForUrl.value(m_nGameType)[0];//创建游戏ID界面

	ui.webEngineView->load(QUrl(sCretaeGidUrl));	//道具服务器
	//ui.webEngineView->load(QUrl("https://www.polchina.com.cn/register/?type=creategid&game=ItemSales"));	//道具服务器


	QString sGid = ui.lineEdit_gid->text();
	m_nBeginNum = ui.lineEdit_beginNum->text().toInt();
	m_createGidTimer.start(1000);
	return;
	/*QString sNewGidNum = QString("%1").arg(m_nBeginNum, m_precisionNum, 10, QLatin1Char('0'));
	sGid += sNewGidNum;

	QString sIDName = "gid_13";
	QString sClickCode = "qt.jQuery('.b form:first').submit()";

	switch (m_nGameType)
	{
	case 13:
	{
		sIDName = "gid_13";
		sClickCode = "qt.jQuery('.b form:first').submit()";
		break;
	}
	case 14: {
		sIDName = "gid_14";
		sClickCode = "qt.jQuery('.b form:eq(2)').submit()";
		break;
	}
	default:
		break;
	}	
	QString sNewGid = QString("qt.jQuery('#%1').val('%2')\n").arg(sIDName).arg(sGid);
	QString sScriptCode;
	sScriptCode += sNewGid;
	sScriptCode += sClickCode;
	ui.webEngineView->page()->runJavaScript(sScriptCode, [](const QVariant& v) { qDebug() << v.toString(); });*/
}

void CGWeb::replyFinished(QNetworkReply* reply)
{
	///最好不要强转，因为可能会修改下载格式
	QTextCodec* codec = QTextCodec::codecForName("utf8");
	//输出
	QString all = codec->toUnicode(reply->readAll());
	qDebug() << all;
	//获取Cookies
	if (reply->hasRawHeader("Set-Cookie"))
	{
		QByteArray cookie = reply->rawHeader("Set-Cookie");
		qDebug() << cookie;
		// 添加了QNetworkCookieJar
			// 可以自己写个类继承QNetworkCookieJar，调用QNetworkCookieJar的allCookies函数来获取cookie列表
	}

}

void CGWeb::onUrlChanged(const QUrl& url)
{
	progress = 100;
	//注入js 
	qDebug() << ui.webEngineView->page()->url().url() << url.url();
	//ui.webEngineView->page()->runJavaScript(jQuery);
	//rotateImages(true);
	QString sCurUrl = ui.webEngineView->page()->url().url();
	if (sCurUrl == "https://www.polchina.com.cn/user/bodymember.php" ||
		sCurUrl.contains("https://www.polchina.com.cn/register/?type=creategid_ok&gid"))//进去后界面，跳转创建用户id界面
	{
		QString sCretaeGidUrl = m_gameTypeForUrl.value(m_nGameType)[0];//创建游戏ID界面

		ui.webEngineView->load(QUrl(sCretaeGidUrl));	//道具服务器
//		ui.webEngineView->load(QUrl("https://www.polchina.com.cn/register/?type=creategid&game=ItemSales"));	//道具服务器
	}


}

void CGWeb::onCheckCreateGid()
{
	if (m_bCreateGid)
	{
		if (m_lastCreateGidTime.elapsed() < 10000)	//10秒检测  卡了重置
			return;
		QString sCretaeGidUrl = m_gameTypeForUrl.value(m_nGameType)[0];//创建游戏ID界面
		ui.webEngineView->load(QUrl(sCretaeGidUrl));	//道具服务器
		//ui.webEngineView->load(QUrl("https://www.polchina.com.cn/register/?type=creategid&game=ItemSales"));	//道具服务器
		m_lastCreateGidTime.restart();
	}
}


void CGWeb::on_comboBox_type_currentIndexChanged(int index)
{
	m_nGameType = ui.comboBox_type->currentData().toInt();
}

void MyPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& msg, int lineNumber, const QString& srcID)
{
	qDebug() << msg;
}


void CGWeb::on_pushButton_myScript_clicked()
{
	//尝试自定义脚本
	/*QString sCode = ui.textEdit->toPlainText();
	ui.webEngineView->page()->runJavaScript(sCode, [](const QVariant& v) { qDebug() << v.toString(); });*/
}
